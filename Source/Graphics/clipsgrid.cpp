#include "clipsgrid.h"

namespace Graphics
{

ClipsGrid::ClipsGrid(const std::shared_ptr<Selection>& currentSelection, QWidget *parent)
    : QWidget(parent), mCurrentSelection(currentSelection)
{
    mPositionBarWidget.setParent(this);
    mSelectionOverlay.setParent(this);

    mCurrentSelection->setSelectionCallback(this);
}

void ClipsGrid::setProject(const std::shared_ptr<Audio::Project>& project)
{
    mProject = project;
    refreshTracks();

    connect(&mPositionBarTimer, &QTimer::timeout, this, &ClipsGrid::drawPositionBar);
    mPositionBarTimer.start(100);
}

void ClipsGrid::refreshBpm(double bpm)
{
    mBpm = bpm;
    mPixelsPerBeat = mZoomLevel * DEFAULT_PIXEL_PER_BEAT_AMOUNT;
    divisionChanged();
    refreshClipsGeometry();
}

void ClipsGrid::refreshTracks()
{
    if (mProject != nullptr) {
        Utils::clearLayout(layout());

        for (const std::weak_ptr<Audio::Track> track : mProject->getTracks()) {
            track.lock()->clipAdded = [=]()
            {
                QPointer<Clip> clipUi = new Clip(this);
                clipUi->show();

                clipUi->setClip(track.lock()->getClips().back());

                clipUi->getClip()->clipMoved = [=]()
                {
                    mProject->updateSavedState(Audio::Project::UNSAVED);
                    refreshClipsGeometry();
                };

                mClips.push_back(clipUi);
                refreshClipsGeometry();

                mProject->updateSavedState(Audio::Project::UNSAVED);
            };
        }

        update();
    }
}

void ClipsGrid::refreshClipsGeometry()
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;

    for (const auto& clipUi : mClips) {
        auto clip = clipUi->getClip();
        double clipPosition = double(clip->getPositionInSamples() / samplesPerMinute) * mBpm * mPixelsPerBeat;
        double clipLength = double(clip->getLengthInSamples() / samplesPerMinute) * mBpm * mPixelsPerBeat;

        clipUi->setGeometry((int) clipPosition,
                            clip->getParentTrack().lock()->getIndex() * (DEFAULT_TRACK_HEIGHT + 1),
                            (int) clipLength,
                            DEFAULT_TRACK_HEIGHT);
    }

    mPositionBarWidget.raise();
    mSelectionOverlay.raise();
}

void ClipsGrid::refreshZoomLevel(double newZoomLevel)
{
    mZoomLevel = newZoomLevel;
    mPixelsPerBeat = mZoomLevel * DEFAULT_PIXEL_PER_BEAT_AMOUNT;
    divisionChanged();
    refreshClipsGeometry();
}

double ClipsGrid::getZoomLevel() const
{
    return mZoomLevel;
}

double ClipsGrid::getPixelsPerBeat() const
{
    return mPixelsPerBeat;
}

double ClipsGrid::getDivision() const
{
    double division = MINIMUM_SPACE_BETWEEN_GRID_LINES / mPixelsPerBeat;
    int index = (int) Utils::search_closest(DEFAULT_DIVISIONS, division);
    division = DEFAULT_DIVISIONS[index];
    return division;
}

int ClipsGrid::roundPosition(int positionInSamples) const
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;

    double samplesInDivision = getDivision() / mBpm * samplesPerMinute;
    int result = int(round(positionInSamples / samplesInDivision) * samplesInDivision);

    return result;
}

void ClipsGrid::drawPositionBar()
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    mPositionBarWidget.barPositionChanged((mProject->getNextReadPosition() / samplesPerMinute) * mBpm * mPixelsPerBeat);
}

void ClipsGrid::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor("#E4E4E7")));

    double division = getDivision();

    for (int i = 0; i * mPixelsPerBeat * division < geometry().width(); i++) {
        p.drawLine(int(i * mPixelsPerBeat * division), 0, int(i * mPixelsPerBeat * division), geometry().height());
    }

    if (mProject != nullptr) {
        for (int i = 0; i < (int) mProject->getTracks().size() + 1; i++) {
            p.drawLine(0, i * (DEFAULT_TRACK_HEIGHT + 1), geometry().width(), i * (DEFAULT_TRACK_HEIGHT + 1));
        }
    }
    p.drawLine(0, geometry().height() - 1, geometry().width(), geometry().height() - 1);
}

void ClipsGrid::resizeEvent(QResizeEvent *)
{
    mPositionBarWidget.setGeometry(0, 0, width(), height());
    mSelectionOverlay.setGeometry(0, 0, width(), height());
    mPositionBarWidget.raise();
    mSelectionOverlay.raise();

    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    setMinimumSize(int((mProject->getTotalLength() / samplesPerMinute) * mBpm * mPixelsPerBeat),
                   150 * ((int) mProject->getTracks().size() + 1));

    update();
}

void ClipsGrid::mousePressEvent(QMouseEvent *event)
{
    clickPosition = event->pos();

    // set position marker
    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    double samplesPerPixel = samplesPerMinute / mBpm / mPixelsPerBeat;

    mProject->setNextReadPosition(roundPosition(event->pos().x() * (int) samplesPerPixel));

    update();

    // selection system
    for (const auto &clip : mClips) {
        if (clip->geometry().contains(event->pos())) {
            mCurrentSelection->setSelectionType(Selection::ClipsSelected);
            mCurrentSelection->addClipToSelection(clip);
            return;
        }
    }

    // if nothing have been selected
    mCurrentSelection->setSelectionType(Selection::NoSelection);
}

void ClipsGrid::mouseReleaseEvent(QMouseEvent *)
{
    if (mMovingClip) {
        double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
        double samplesPerPixel = samplesPerMinute / mBpm / mPixelsPerBeat;

        double newClipPosition = mMovingClip->x() * samplesPerPixel;
        mMovingClip->getClip()->setClipPositionInSamples(newClipPosition);
        mMovingClip = nullptr;

    }
}

void ClipsGrid::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::LeftButton)) {
        if (event->pos().y() > (DEFAULT_TRACK_HEIGHT + 1) * (int) mProject->getTracks().size() - 1)
            return;

        int samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
        int samplesPerPixel = samplesPerMinute / (int) mBpm / (int) mPixelsPerBeat;

        // move clip if needed
        if (mMovingClip == nullptr) {
            for (const auto& clip : mClips) {
                if ((clip->shouldMoveClip(clickPosition) && clip->geometry().contains(clickPosition))) {
                    mMovingClip = clip.get();
                }
            }
        }

        if (mMovingClip != nullptr) {
            double clipPosition =
                double(mMovingClip->getClip()->getPositionInSamples() / (double) samplesPerMinute) * mBpm * mPixelsPerBeat;

            double newPosition = clipPosition
                + (double) roundPosition(int(event->position().x() - clickPosition.x()) * samplesPerPixel) / samplesPerMinute * mBpm
                    * mPixelsPerBeat;

            mMovingClip->setGeometry((int) newPosition, mMovingClip->y(), mMovingClip->width(), mMovingClip->height());
            return;
        }

        // else set selection area
        Selection::SelectionArea area = mCurrentSelection->getSelectedArea();
        if (mCurrentSelection->getSelectionType() != Selection::AreaSelected) {
            mCurrentSelection->setSelectionType(Selection::AreaSelected);
            area.startTrackIndex = event->pos().y() / (DEFAULT_TRACK_HEIGHT + 1);
            area.startSample = roundPosition(event->pos().x() * samplesPerPixel);
        }

        area.nbTracks = event->pos().y() / (DEFAULT_TRACK_HEIGHT + 1) - area.startTrackIndex;
        area.nbSamples = roundPosition(event->pos().x() * samplesPerPixel) - area.startSample;
        mCurrentSelection->setSelectedArea(area);
    }
}

void ClipsGrid::selectionChanged()
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;

    Selection::SelectionArea area = mCurrentSelection->getSelectedArea();

    int x = int((area.startSample / samplesPerMinute) * mBpm * mPixelsPerBeat);
    int w = int((area.nbSamples / samplesPerMinute) * mBpm * mPixelsPerBeat);
    int y, h;

    if (area.nbTracks >= 0) {
        y = area.startTrackIndex * (DEFAULT_TRACK_HEIGHT + 1);
        h = (area.nbTracks + 1) * (DEFAULT_TRACK_HEIGHT + 1);
    }

    else {
        y = (area.startTrackIndex + 1) * (DEFAULT_TRACK_HEIGHT + 1);
        h = (area.nbTracks - 1) * (DEFAULT_TRACK_HEIGHT + 1);
    }

    mSelectionOverlay.areaChanged(QRect(x, y, w, h));

    mPositionBarWidget.raise();
    mSelectionOverlay.raise();
}

} // namespace Graphics

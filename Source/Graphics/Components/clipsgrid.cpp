#include "clipsgrid.h"

namespace Graphics
{

ClipsGrid::ClipsGrid(const QSharedPointer<Audio::Project> &project,
                     const QSharedPointer<TimelineProperties> &timelineProperties,
                     QWidget *parent)
    : QWidget(parent), mProject(project), mTimelineProperties(timelineProperties)
{
    mPositionBarWidget.setParent(this);
    mSelectionOverlay.setParent(this);

    setupCallbacks();
}

void ClipsGrid::setupCallbacks()
{
    connect(mProject->getProjectProperties().get(), &Audio::ProjectProperties::bpmChanged, [=]()
    {
        updateClipsGeometry();
    });

    connect(mTimelineProperties.get(), &TimelineProperties::pixelsPerBeatAmountChanged, [=]()
    {
        updateClipsGeometry();
    });

    connect(mTimelineProperties.get(), &TimelineProperties::currentSelectionChanged, [=]()
    {
        updateSelectionOverlay();
    });

    connect(mProject.get(), &Audio::Project::trackAdded, [=](int trackIndex)
    {
        setupTrackCallbacks(mProject->getTrackByIndex(trackIndex).get());
        update();
    });

    connect(&mPositionBarTimer, &QTimer::timeout, this, &ClipsGrid::drawPositionBar);
    mPositionBarTimer.start(100);
}

void ClipsGrid::setupTrackCallbacks(const QPointer<Audio::Track> &track)
{
    connect(track.get(), &Audio::Track::clipAdded, [=](int clipIndex)
    {
        QPointer<Clip> clipUi = new Clip(this);
        clipUi->show();

        auto newClip = track->getClips().at(clipIndex);
        clipUi->setClip(newClip);
        setupClipCallbacks(newClip.get());

        mClips.insert(clipIndex, clipUi);

        updateClipsGeometry();
    });

    connect(track.get(), &Audio::Track::clipRemoved, [=](int clipIndex)
    {
        mClips.at(clipIndex)->deleteLater();
        mClips.remove(clipIndex);
    });
}

void ClipsGrid::setupClipCallbacks(const QPointer<Audio::Clip> &clip)
{
    connect(clip->getClipProperties().get(), &Audio::ClipProperties::clipMoved, [=]()
    {
        updateClipsGeometry();
    });
}

void ClipsGrid::updateClipsGeometry()
{
    for (const auto &clipUi: mClips) {
        auto clipProperties = clipUi->getClip()->getClipProperties();

        int clipPosition = samplesToPixels((int) clipProperties->getPositionInSamples());
        int clipLength = samplesToPixels((int) clipProperties->getLengthInSamples());

        clipUi->setGeometry(clipPosition,
                            clipProperties->getParentTrack()->getTrackProperties()->getIndex()
                                * (DEFAULT_TRACK_HEIGHT + 1),
                            clipLength,
                            DEFAULT_TRACK_HEIGHT);
    }

    mPositionBarWidget.raise();
    mSelectionOverlay.raise();
}

void ClipsGrid::updateSelectionOverlay()
{
    Selection::SelectionArea area = mTimelineProperties->getCurrentSelection()->getSelectedArea();

    int x = samplesToPixels(area.startSample);
    int w = samplesToPixels(area.nbSamples);
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
    mSelectionOverlay.raise();
}

double ClipsGrid::getDivision() const
{
    double division = MINIMUM_SPACE_BETWEEN_GRID_LINES / mTimelineProperties->getPixelsPerBeatAmount();
    int index = (int) Utils::search_closest(DEFAULT_DIVISIONS, division);
    division = DEFAULT_DIVISIONS[index];
    return division;
}

int ClipsGrid::roundPosition(int positionInSamples) const
{
    int samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;

    int samplesInDivision = int(getDivision() / mProject->getProjectProperties()->getBpm() * (double) samplesPerMinute);
    int result = (int) round((double) positionInSamples / samplesInDivision) * samplesInDivision;

    return result;
}

int ClipsGrid::samplesToPixels(int samples) const
{
    int samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    int pixelsPerMinute =
        int(mProject->getProjectProperties()->getBpm() * mTimelineProperties->getPixelsPerBeatAmount());

    return int((double) samples / (double) samplesPerMinute * (double) pixelsPerMinute);
}

int ClipsGrid::pixelsToSamples(int pixels) const
{
    int samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    int samplesPerPixel = int((double) samplesPerMinute / mProject->getProjectProperties()->getBpm()
                                  / mTimelineProperties->getPixelsPerBeatAmount());

    return samplesPerPixel * pixels;
}

void ClipsGrid::drawPositionBar()
{
    mPositionBarWidget.barPositionChanged(samplesToPixels((int) mProject->getNextReadPosition()));
}

void ClipsGrid::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor("#E4E4E7")));

    int pixelsInDivision = int(mTimelineProperties->getPixelsPerBeatAmount() * getDivision());
    for (int i = 0; i * pixelsInDivision < geometry().width(); i++) {
        p.drawLine(i * pixelsInDivision, 0, i * pixelsInDivision, geometry().height());
    }

    for (int i = 0; i < (int) mProject->getTracks().size() + 1; i++) {
        p.drawLine(0, i * (DEFAULT_TRACK_HEIGHT + 1), geometry().width(), i * (DEFAULT_TRACK_HEIGHT + 1));
    }

    p.drawLine(0, geometry().height() - 1, geometry().width(), geometry().height() - 1);
}

void ClipsGrid::resizeEvent(QResizeEvent *)
{
    mPositionBarWidget.setGeometry(0, 0, width(), height());
    mSelectionOverlay.setGeometry(0, 0, width(), height());
    mPositionBarWidget.raise();
    mSelectionOverlay.raise();

    setMinimumSize(samplesToPixels((int) mProject->getTotalLength()),
                   150 * ((int) mProject->getTracks().size() + 1));

    update();
}

void ClipsGrid::mousePressEvent(QMouseEvent *event)
{
    clickPosition = event->pos();

    mProject->setNextReadPosition(roundPosition(pixelsToSamples(event->pos().x())));

    // selection system
    for (const auto &clip: mClips) {
        if (clip->geometry().contains(event->pos())) {
            mTimelineProperties->getCurrentSelection()->setSelectionType(Selection::ClipsSelected);
            mTimelineProperties->getCurrentSelection()->addClipToSelection(clip);
            return;
        }
    }

    // if nothing have been selected
    mTimelineProperties->getCurrentSelection()->setSelectionType(Selection::NoSelection);
}

void ClipsGrid::mouseReleaseEvent(QMouseEvent *)
{
    if (mMovingClip) {
        mMovingClip->getClip()->getClipProperties()->setPositionInSamples(pixelsToSamples(mMovingClip->x()));
        mMovingClip = nullptr;
    }
}

void ClipsGrid::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::LeftButton)) {
        if (event->pos().y() > (DEFAULT_TRACK_HEIGHT + 1) * (int) mProject->getTracks().size() - 1)
            return;

        // move clip if needed
        if (mMovingClip == nullptr) {
            for (const auto &clip: mClips) {
                if ((clip->shouldMoveClip(clickPosition) && clip->geometry().contains(clickPosition))) {
                    mMovingClip = clip.get();
                }
            }
        }

        if (mMovingClip != nullptr) {
            int clipPosition = (int) mMovingClip->getClip()->getClipProperties()->getPositionInSamples();

            int newPositionInSamples =
                roundPosition(clipPosition + pixelsToSamples((int) event->position().x() - clickPosition.x()));

            mMovingClip->setGeometry(samplesToPixels(newPositionInSamples),
                                     mMovingClip->y(),
                                     mMovingClip->width(),
                                     mMovingClip->height());
            return;
        }

        // else set selection area
        Selection::SelectionArea area = mTimelineProperties->getCurrentSelection()->getSelectedArea();
        if (mTimelineProperties->getCurrentSelection()->getSelectionType() != Selection::AreaSelected) {
            mTimelineProperties->getCurrentSelection()->setSelectionType(Selection::AreaSelected);
            area.startTrackIndex = event->pos().y() / (DEFAULT_TRACK_HEIGHT + 1);
            area.startSample = roundPosition(pixelsToSamples(event->pos().x()));
        }

        area.nbTracks = event->pos().y() / (DEFAULT_TRACK_HEIGHT + 1) - area.startTrackIndex;
        area.nbSamples = roundPosition(pixelsToSamples(event->pos().x())) - area.startSample;
        mTimelineProperties->getCurrentSelection()->setSelectedArea(area);
    }
}

} // namespace Graphics

#include "clipsgrid.h"

namespace Graphics {

ClipsGrid::ClipsGrid(std::shared_ptr<Selection> currentSelection, QWidget *parent) : QWidget(parent), mCurrentSelection(currentSelection)
{
    mPositionBarWidget.setParent(this);
    mSelectionOverlay.setParent(this);

    mCurrentSelection->setSelectionCallback(this);

    updateGeometry();
}

void ClipsGrid::refreshBpm(double bpm)
{
    mBpm = bpm;
    mPixelsPerBeat = mZoomLevel * DEFAULT_PIXEL_PER_BEAT_AMOUNT;
    refreshClips();
}

void ClipsGrid::setProject(std::shared_ptr<Audio::Project> project)
{
    mProject = project;
    refreshClips();

    connect(&mPositionBarTimer, &QTimer::timeout, this, &ClipsGrid::drawPositionBar);
    mPositionBarTimer.start(100);
}

void ClipsGrid::refreshClips()
{
    mClips.clear();
    if (mProject.get() != nullptr)
    {
        int i = 0;
        Utils::clearLayout(layout(), true);

        for (auto track : mProject->getTracks())
        {
            for (auto clip : track->getClips())
            {
                auto clipUi = std::make_shared<Clip>();
                clipUi->setParent(this);
                clipUi->setClip(clip);

                clipUi->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

                mClips.append(clipUi);
                clipUi->show();
            }
            i++;
        }

        double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;

        for (auto clipUi : mClips)
        {
            auto clip = clipUi->getAudioClip();
            double clipPosition = double(clip->getPositionInSamples()/samplesPerMinute)*mBpm*mPixelsPerBeat;
            double clipLength = double(clip->getLengthInSamples()/samplesPerMinute)*mBpm*mPixelsPerBeat;

            clipUi->setGeometry(clipPosition, clip->getParentTrack()->getIndex()*(DEFAULT_TRACK_HEIGHT+1), clipLength, DEFAULT_TRACK_HEIGHT);
        }
        mPositionBarWidget.raise();
        mSelectionOverlay.raise();

        update();
    }
}

void ClipsGrid::refreshZoomLevel(double newZoomLevel)
{
    mZoomLevel = newZoomLevel;
    mPixelsPerBeat = mZoomLevel * DEFAULT_PIXEL_PER_BEAT_AMOUNT;
    refreshClips();
}

double ClipsGrid::getZoomLevel() const
{
    return mZoomLevel;
}

double ClipsGrid::getDivision() const
{
    double division = MINIMUM_SPACE_BETWEEN_GRID_LINES / mPixelsPerBeat;
    int index = Utils::search_closest(DEFAULT_DIVISIONS, division);
    division = DEFAULT_DIVISIONS[index];
    return division;
}

int ClipsGrid::roundPosition(int positionInSamples) const
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;

    double samplesInDivision = getDivision() / mBpm * samplesPerMinute;
    int result = round(positionInSamples / samplesInDivision) * samplesInDivision;

    return result;
}

void ClipsGrid::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor("#E4E4E7")));

    double division = getDivision();

    for (int i = 0; i*mPixelsPerBeat*division < geometry().width(); i++)
    {
        p.drawLine(i*mPixelsPerBeat*division, 0, i*mPixelsPerBeat*division, geometry().height());
    }
    if (mProject.get() != nullptr)
    {
        for (int i = 0; i < mProject->getTracks().size()+1; i++)
        {
            p.drawLine(0, i*(DEFAULT_TRACK_HEIGHT+1), geometry().width(), i*(DEFAULT_TRACK_HEIGHT+1));
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

    double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;
    setMinimumSize((mProject->getTotalLength()/samplesPerMinute)*mBpm*mPixelsPerBeat, 150 * (mProject->getTracks().size() + 1));

    update();
}

void ClipsGrid::drawPositionBar()
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;
    mPositionBarWidget.barPositionChanged((mProject->getNextReadPosition()/samplesPerMinute)*mBpm*mPixelsPerBeat);
}

void ClipsGrid::mousePressEvent(QMouseEvent *event)
{
    // set position marker
    double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;
    double samplesPerPixel = samplesPerMinute/mBpm/mPixelsPerBeat;

    mProject->setNextReadPosition(roundPosition(event->pos().x() * samplesPerPixel));

    update();

    // selection system
    for (const auto &clip : mClips)
    {
        if (clip->geometry().contains(event->pos()))
        {
            mCurrentSelection->setSelectionType(Selection::ClipsSelected);
            mCurrentSelection->addClipToSelection(clip);
            return;
        }
    }

    // if nothing have been selected
    mCurrentSelection->setSelectionType(Selection::NoSelection);
}

void ClipsGrid::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::LeftButton))
    {
        if (event->pos().y() > (DEFAULT_TRACK_HEIGHT + 1) * mProject->getTracks().size()-1)
            return;

        double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;
        double samplesPerPixel = samplesPerMinute/mBpm/mPixelsPerBeat;

        Selection::SelectionArea area = mCurrentSelection->getSelectedArea();
        if (mCurrentSelection->getSelectionType() != Selection::AreaSelected)
        {
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
    // draw selection
    double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;

    Selection::SelectionArea area = mCurrentSelection->getSelectedArea();

    int x = double(area.startSample / samplesPerMinute) * mBpm * mPixelsPerBeat;
    int w = double(area.nbSamples / samplesPerMinute) * mBpm * mPixelsPerBeat;
    int y, h;

    if (area.nbTracks >= 0)
    {
        y = area.startTrackIndex * (DEFAULT_TRACK_HEIGHT+1);
        h = (area.nbTracks + 1) * (DEFAULT_TRACK_HEIGHT+1);
    }

    else
    {
        y = (area.startTrackIndex + 1) * (DEFAULT_TRACK_HEIGHT+1);
        h = (area.nbTracks - 1) * (DEFAULT_TRACK_HEIGHT+1);
    }

    mSelectionOverlay.areaChanged(QRect(x, y, w, h));

    mPositionBarWidget.raise();
    mSelectionOverlay.raise();
}

} // namespace Graphics

#include "clipsgrid.h"

namespace Graphics {

ClipsGrid::ClipsGrid(QWidget *parent) : QWidget(parent)
{
    setLayout(new QGridLayout());
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(1);

    mPositionBarWidget.setParent(this);
    mPositionBarWidget.setGeometry(geometry());

    setStyleSheet("background-color: #EEEEEE;");
}

void ClipsGrid::refreshBpm(double bpm)
{
    mBpm = bpm;
    mPixelsPerBeat = mZoomLevel * DEFAULT_PIXEL_PER_BEAT_AMOUNT;
    refreshClips();
}

void ClipsGrid::setProject(std::shared_ptr<Audio::Project> project)
{
    mAudioTimeline = project;
    refreshClips();

    connect(&mPositionBarTimer, &QTimer::timeout, this, &ClipsGrid::drawPositionBar);
    mPositionBarTimer.start(50);
}

void ClipsGrid::refreshClips()
{
    mClips.clear();
    if (mAudioTimeline.get() != nullptr)
    {
        int i = 0;
        Utils::clearLayout(layout(), true);

        for (auto track : mAudioTimeline->getTracks())
        {
            for (auto clip : track->getClips())
            {
                auto clipUi = std::make_shared<Clip>();
                clipUi->setClip(clip);

                clipUi->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

                ((QGridLayout*)layout())->addWidget(clipUi.get(), i, 0);
                layout()->setAlignment(clipUi.get(), Qt::AlignTop | Qt::AlignLeft);

                mClips.append(clipUi);
            }
            i++;
        }

        QWidget *spacerWidget = new QWidget();
        spacerWidget->setStyleSheet("background-color: rgba(0,0,0,0)");
        spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ((QGridLayout*) layout())->addWidget(spacerWidget, i, 0);

        double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;

        for (auto clipUi : mClips)
        {
            auto clip = clipUi->getAudioClip();
            double clipPosition = double(clip->getPositionInSamples()/samplesPerMinute)*mBpm*mPixelsPerBeat;
            double clipLength = double(clip->getLengthInSamples()/samplesPerMinute)*mBpm*mPixelsPerBeat;

            clipUi->setGeometry(clipPosition, clip->getParentTrack()->getIndex()*(DEFAULT_TRACK_HEIGHT+1)+1, clipLength, DEFAULT_TRACK_HEIGHT);
            clipUi->setMinimumSize(clipLength, DEFAULT_TRACK_HEIGHT);
        }

        updateGeometry();
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

void ClipsGrid::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor("#BDBDBD")));

    double division = getDivision();

    for (int i = 0; i*mPixelsPerBeat*division < geometry().width(); i++)
    {
        p.drawLine(i*mPixelsPerBeat*division, 0, i*mPixelsPerBeat*division, geometry().height());
    }
    if (mAudioTimeline.get() != nullptr)
    {
        for (int i = 0; i < mAudioTimeline->getTracks().size()+1; i++)
        {
            p.drawLine(0, i*(DEFAULT_TRACK_HEIGHT+1), geometry().width(), i*(DEFAULT_TRACK_HEIGHT+1));
        }
    }
    p.drawLine(0, geometry().height() - 1, geometry().width(), geometry().height() - 1);
}

void ClipsGrid::resizeEvent(QResizeEvent *event)
{
    //refreshClips();
    mPositionBarWidget.setGeometry(geometry());

    double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;
    setMinimumSize((mAudioTimeline->getTotalLength()/samplesPerMinute)*mBpm*mPixelsPerBeat, 150 * (mAudioTimeline->getTracks().size() + 1));

    mPositionBarWidget.raise();
    update();
}

void ClipsGrid::drawPositionBar()
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;
    mPositionBarWidget.barPositionChanged((mAudioTimeline->getNextReadPosition()/samplesPerMinute)*mBpm*mPixelsPerBeat);
}

void ClipsGrid::mousePressEvent(QMouseEvent *event)
{
    double selection;
    double samplesPerMinute = DEFAULT_SAMPLE_RATE*60;
    double samplesPerPixel = samplesPerMinute/mBpm/mPixelsPerBeat;
    selection = event->x() * samplesPerPixel;

    double samplesInDivision = getDivision() / mBpm * samplesPerMinute;
    selection = round(selection / samplesInDivision) * samplesInDivision;

    mAudioTimeline->setNextReadPosition(selection);

    update();
}

} // namespace Graphics

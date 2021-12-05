#include "graphics_timeline.h"

namespace Graphics {

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
    setLayout(&mMainLayout);
    layout()->setSpacing(1);
}

void Timeline::setAudioTimeline(std::shared_ptr<Audio::Timeline> timeline)
{
    mAudioTimeline = timeline;

    displayTracks();
}

void Timeline::displayTracks()
{
    Utils::clearLayout(&mMainLayout);
    int i = 0;

    for (auto track : mAudioTimeline->getTracks())
    {
        mMainLayout.addWidget(new Graphics::Track(track), i, 0, Qt::AlignTop);
        i++;
    }

    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mMainLayout.addWidget(spacerWidget, i, 0);

    mMainLayout.addWidget(&mScrollArea, 0, 1, i+1, 1);
    mScrollArea.setWidget(&mClipsGrid);

    mClipsGrid.setGeometry(mScrollArea.geometry());
    mClipsGrid.refreshBpm(mAudioTimeline->getBpm());
    mClipsGrid.setAudioTimeline(mAudioTimeline);
}

void Timeline::refreshZoomLevel(double newZoomLevel)
{
    mZoomLevel = newZoomLevel;
    mClipsGrid.refreshZoomLevel(mZoomLevel);

    refreshClipsGridGeometry();
}

double Timeline::getZoomLevel() const
{
    return mZoomLevel;
}

void Timeline::refreshClipsGridGeometry()
{
    if (mScrollArea.width() > mClipsGrid.minimumWidth())
        mClipsGrid.setGeometry(0, 0, mScrollArea.width(), mClipsGrid.height());

    if (mScrollArea.height() > mClipsGrid.minimumHeight())
        mClipsGrid.setGeometry(0, 0, mClipsGrid.width(), mScrollArea.height());
}

void Timeline::refreshBpm()
{
    if (mAudioTimeline.get() != nullptr)
        mClipsGrid.refreshBpm(mAudioTimeline->getBpm());
}

double Timeline::getDivision() const
{
    return mClipsGrid.getDivision();
}

void Timeline::resizeEvent(QResizeEvent *event)
{
    refreshClipsGridGeometry();
}

}

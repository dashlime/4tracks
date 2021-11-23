#include "graphics_timeline.h"

namespace Graphics {

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
    setLayout(&mMainLayout);
}

void Timeline::setAudioTimeline(std::shared_ptr<Audio::Timeline> timeline)
{
    mAudioTimeline = timeline;

    displayTracks();
}

void Timeline::displayTracks()
{
    while (mMainLayout.takeAt(0) != nullptr)
    {
        mMainLayout.removeWidget(mMainLayout.takeAt(0)->widget());
    }

    int i = 0;

    for (auto track : mAudioTimeline->getTracks())
    {
        mMainLayout.addWidget(new Graphics::Track(track), i, 0, Qt::AlignTop);
        i++;
    }

    mMainLayout.addWidget(&mScrollArea, 0, 1, 1, 1);
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

void Timeline::resizeEvent(QResizeEvent *event)
{
    refreshClipsGridGeometry();
}

}

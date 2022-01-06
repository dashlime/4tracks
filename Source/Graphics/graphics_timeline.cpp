#include "graphics_timeline.h"

namespace Graphics {

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
    setLayout(&mMainLayout);
    layout()->setSpacing(1);
    layout()->setContentsMargins(10, 10, 0, 0);
}

void Timeline::setProject(std::shared_ptr<Audio::Project> project)
{
    mProject = project;

    displayTracks();
}

void Timeline::displayTracks()
{
    Utils::clearLayout(&mMainLayout);
    int i = 0;

    for (auto track : mProject->getTracks())
    {
        mMainLayout.addWidget(new Graphics::Track(track), i, 0, Qt::AlignTop);
        i++;
    }

    QWidget *spacerWidget = new QWidget();
    spacerWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    mMainLayout.addWidget(spacerWidget, i, 0);

    mMainLayout.addWidget(&mClipsGrid, 0, 1, i+1, 1);

    mClipsGrid.refreshBpm(mProject->getBpm());
    mClipsGrid.setProject(mProject);
}

void Timeline::refreshZoomLevel(double newZoomLevel)
{
    mZoomLevel = newZoomLevel;
    mClipsGrid.refreshZoomLevel(mZoomLevel);
}

double Timeline::getZoomLevel() const
{
    return mZoomLevel;
}

void Timeline::refreshBpm()
{
    if (mProject.get() != nullptr)
        mClipsGrid.refreshBpm(mProject->getBpm());
}

double Timeline::getDivision() const
{
    return mClipsGrid.getDivision();
}

void Timeline::resizeEvent(QResizeEvent *event)
{
    setMinimumSize(DEFAULT_TRACK_HEIGHT, mClipsGrid.minimumHeight());
}

void Timeline::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}

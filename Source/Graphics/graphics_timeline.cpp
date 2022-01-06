#include "graphics_timeline.h"

namespace Graphics {

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
    setLayout(&mMainLayout);
    layout()->setSpacing(0);
    layout()->setContentsMargins(10, 10, 0, 0);

    mTracksWidget.setLayout(&mTracksLayout);
    mTracksWidget.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    layout()->addWidget(&mTracksWidget);
    layout()->addWidget(&mClipsGrid);

    mTracksLayout.setSpacing(1);
    mTracksLayout.setContentsMargins(0, 0, 0, 0);

    mAddTrackButton.setIcon(QIcon(":/icons/plus_button.png"));
    mAddTrackButton.setFixedSize(150, DEFAULT_TRACK_HEIGHT);

    connect(&mAddTrackButton, &QPushButton::pressed, [=]() {
        mProject->addTrack(std::make_shared<Audio::AudioTrack>("Track", mProject->getTracks().size()));
    });

    mSpacerWidget.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

void Timeline::setProject(std::shared_ptr<Audio::Project> project)
{
    mProject = project;

    displayTracks();
}

void Timeline::displayTracks()
{
    Utils::clearLayout(&mTracksLayout);
    int i = 0;

    for (auto track : mTracks)
    {
        delete track;
    }
    mTracks.clear();

    for (auto track : mProject->getTracks())
    {
        mTracks.append(new Graphics::Track(track));
        mTracksLayout.addWidget(mTracks.last(), Qt::AlignTop);
        i++;
    }

    mTracksLayout.addWidget(&mAddTrackButton, Qt::AlignTop);

    mTracksLayout.addWidget(&mSpacerWidget, Qt::AlignTop);

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
    setMinimumSize(150, mClipsGrid.minimumHeight());
    mMainLayout.setGeometry(geometry());
}

void Timeline::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}

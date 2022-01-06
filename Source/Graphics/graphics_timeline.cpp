#include "graphics_timeline.h"

namespace Graphics {

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
    setLayout(&mMainLayout);
    layout()->setSpacing(1);
    layout()->setContentsMargins(10, 10, 0, 0);

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
    Utils::clearLayout(&mMainLayout);
    int i = 0;

    for (auto track : mTracks)
    {
        delete track;
    }
    mTracks.clear();

    for (auto track : mProject->getTracks())
    {
        mTracks.append(new Graphics::Track(track));
        mMainLayout.addWidget(mTracks.last(), i, 0, Qt::AlignTop);
        i++;
    }

    mMainLayout.addWidget(&mAddTrackButton, i, 0);

    mMainLayout.addWidget(&mSpacerWidget, i+1, 0);

    mMainLayout.addWidget(&mClipsGrid, 0, 1, i+2, 1);

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

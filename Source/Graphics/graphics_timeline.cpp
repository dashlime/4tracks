#include "graphics_timeline.h"

namespace Graphics {

Timeline::Timeline(QWidget *parent) : QWidget(parent)
{
    // objects initialization
    mCurrentSelection = std::make_shared<Selection>();
    mClipsGrid = std::make_unique<ClipsGrid>(mCurrentSelection);
    mDivisionsMarker = std::make_unique<DivisionsMarker>(mClipsGrid->getDivision(), mClipsGrid->getPixelsPerBeat());

    mAddTrackButton.setFocusPolicy(Qt::NoFocus);
    mAddTrackButton.setIcon(QIcon(":/icons/plus_button.png"));
    mAddTrackButton.setFixedSize(150, DEFAULT_TRACK_HEIGHT);

    mTracksSpacer.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    mDivisionsMarkerSpacer.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mDivisionsMarkerSpacer.setFixedSize(150 + 1, mDivisionsMarker->height());

    mVerticalScrollView.setWidget(&mScrollWidget);
    mVerticalScrollView.setWidgetResizable(true);
    mVerticalScrollView.setFocusPolicy(Qt::FocusPolicy::NoFocus);
    mVerticalScrollView.setFocus(Qt::NoFocusReason);
    mVerticalScrollView.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // layouts initialization
    setLayout(&mMainLayout);
    layout()->setSpacing(0);
    layout()->setContentsMargins(10, 3, 3, 3);

    mTracksLayout.setSpacing(1);
    mTracksLayout.setContentsMargins(0, 0, 0, 0);

    mTracksWidget.setLayout(&mTracksLayout);
    mTracksWidget.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    mDivisionsMarkerLayout.setSpacing(0);
    mDivisionsMarkerLayout.setContentsMargins(0, 0, 0, 0);

    mDivisionsMarkerWidget.setLayout(&mDivisionsMarkerLayout);
    mDivisionsMarkerWidget.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    mScrollLayout.setSpacing(2);
    mScrollLayout.setContentsMargins(0, 0, 0, 0);

    mScrollWidget.setLayout(&mScrollLayout);

    layout()->addWidget(&mDivisionsMarkerWidget);
    layout()->addWidget(&mVerticalScrollView);

    mDivisionsMarkerLayout.addWidget(&mDivisionsMarkerSpacer);
    mDivisionsMarkerLayout.addWidget(mDivisionsMarker.get());

    mScrollLayout.addWidget(&mTracksWidget);
    mScrollLayout.addWidget(mClipsGrid.get());

    // connect signals
    connect(&mAddTrackButton, &QPushButton::pressed, [=]() {
        mProject->addTrack(std::make_shared<Audio::AudioTrack>("Track", mProject->getTracks().size()));
    });

    mClipsGrid->onDivisionChanged = [=]() {
        mDivisionsMarker->refresh(mClipsGrid->getDivision(), mClipsGrid->getPixelsPerBeat());
    };
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
        track.reset();

    mTracks.clear();

    for (auto track : mProject->getTracks())
    {
        mTracks.push_back(std::make_shared<Graphics::Track>(track));
        mTracksLayout.addWidget(mTracks.back().get(), Qt::AlignTop);
        i++;
    }

    mTracksLayout.addWidget(&mAddTrackButton, Qt::AlignTop);

    mTracksLayout.addWidget(&mTracksSpacer, Qt::AlignTop);

    mClipsGrid->refreshBpm(mProject->getBpm());
    mClipsGrid->setProject(mProject);
}

void Timeline::refreshZoomLevel(double newZoomLevel)
{
    mZoomLevel = newZoomLevel;
    mClipsGrid->refreshZoomLevel(mZoomLevel);
}

double Timeline::getZoomLevel() const
{
    return mZoomLevel;
}

void Timeline::refreshBpm()
{
    if (mProject.get() != nullptr)
        mClipsGrid->refreshBpm(mProject->getBpm());
}

double Timeline::getDivision() const
{
    return mClipsGrid->getDivision();
}

void Timeline::resizeEvent(QResizeEvent *)
{
    setMinimumSize(150, 200);
    mMainLayout.setGeometry(QRect(0, 0, width(), height()));
}

void Timeline::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Timeline::mousePressEvent(QMouseEvent *event)
{
    for (const auto &track : mTracks)
    {
        if (track->geometry().contains(event->pos()))
        {
            mCurrentSelection->setSelectionType(Selection::TracksSelected);
            mCurrentSelection->addTrackToSelection(track);
            return;
        }
    }

    // nothing was selected
    mCurrentSelection->setSelectionType(Selection::NoSelection);
}

}

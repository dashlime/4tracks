#include "timeline.h"

namespace Graphics
{

Timeline::Timeline(const QSharedPointer<Audio::Project> &project, QWidget *parent)
    : QWidget(parent), mProject(project),
      mTimelineProperties(new TimelineProperties(QSharedPointer<Selection>::create())),
      mClipsGrid(new ClipsGrid(mProject, mTimelineProperties))
{
    setFocusPolicy(Qt::ClickFocus);
    setMinimumSize(150, 200);

    setupComponents();
    setupLayouts();

    setupCallbacks();
}

QSharedPointer<TimelineProperties> Timeline::getTimelineProperties() const
{
    return mTimelineProperties;
}

void Timeline::setupComponents()
{
    mDivisionsMarker = new DivisionsMarker(mClipsGrid->getDivision(), mTimelineProperties->getPixelsPerBeatAmount());

    mAddTrackButton.setFocusPolicy(Qt::NoFocus);
    mAddTrackButton.setIcon(QIcon(":/icons/plus_button.png"));
    mAddTrackButton.setFixedSize(150, DEFAULT_TRACK_HEIGHT);

    connect(&mAddTrackButton, &QPushButton::pressed, [=]()
    {
        mProject->createTrack("Track");
    });

    mTracksSpacer.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    mDivisionsMarkerSpacer.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mDivisionsMarkerSpacer.setFixedSize(150 + 1, mDivisionsMarker->height());

    mVerticalScrollView.setWidget(&mScrollWidget);
    mVerticalScrollView.setWidgetResizable(true);
    mVerticalScrollView.setFocusPolicy(Qt::FocusPolicy::NoFocus);
    mVerticalScrollView.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Timeline::setupLayouts()
{
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
}

void Timeline::setupCallbacks()
{
    connect(mProject.get(), &Audio::Project::trackAdded, [=](int index)
    {
        mTracks.insert(index, new Track(mProject->getTrackByIndex(index)));
        refreshTracksLayout();
    });

    connect(mProject.get(), &Audio::Project::trackRemoved, [=](int index)
    {
        mTracks.remove(index);
        refreshTracksLayout();
    });

    connect(mTimelineProperties.get(), &TimelineProperties::zoomLevelChanged, [=]()
    {
        mDivisionsMarker->refresh(mClipsGrid->getDivision(), mTimelineProperties->getPixelsPerBeatAmount());
    });
}

void Timeline::refreshTracksLayout()
{
    Utils::clearLayout(&mTracksLayout);

    for (const auto &track: mTracks) {
        mTracksLayout.addWidget(track, Qt::AlignTop);
    }

    mTracksLayout.addWidget(&mAddTrackButton, Qt::AlignTop);
    mTracksLayout.addWidget(&mTracksSpacer, Qt::AlignTop);
}

void Timeline::resizeEvent(QResizeEvent *)
{
    mMainLayout.setGeometry(QRect(0, 0, width(), height()));
}

void Timeline::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Timeline::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::LeftButton)) {
        for (const auto &track: mTracks) {
            if (track->geometry().contains(event->pos())) {
                mTimelineProperties->getCurrentSelection()->setSelectionType(Selection::TracksSelected);
                mTimelineProperties->getCurrentSelection()->addTrackToSelection(track);
                return;
            }
        }

        // nothing was selected
        mTimelineProperties->getCurrentSelection()->setSelectionType(Selection::NoSelection);
    }
}

}

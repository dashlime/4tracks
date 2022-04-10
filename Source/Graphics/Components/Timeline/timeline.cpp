#include "timeline.h"

namespace Graphics
{

Timeline::Timeline(const QSharedPointer<Audio::Project> &project, QWidget *parent)
    : QWidget(parent), mProject(project),
      mTimelineProperties(new TimelineProperties(QSharedPointer<SelectionManager>::create())),
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

QPointer<ClipsGrid> Timeline::getClipsGrid() const
{
    return mClipsGrid;
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
    mVerticalScrollView.horizontalScrollBar()->hide();
    mVerticalScrollView.horizontalScrollBar()->setEnabled(false);
    mVerticalScrollView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mClipsGridViewport.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mClipsGrid->setParent(&mClipsGridViewport);
    mClipsGrid->show();

    mDivisionsMarkerViewport.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mDivisionsMarker->setParent(&mDivisionsMarkerViewport);
    mDivisionsMarker->show();
}

void Timeline::setupLayouts()
{
    setLayout(&mMainLayout);
    layout()->setSpacing(0);
    layout()->setContentsMargins(10, 3, 3, 3);

    mTracksLayout.setSpacing(1);
    mTracksLayout.setContentsMargins(0, 0, 0, 0);

    mTracksLayout.addWidget(&mAddTrackButton, Qt::AlignTop);
    mTracksLayout.addWidget(&mTracksSpacer, Qt::AlignTop);

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
    mDivisionsMarkerLayout.addWidget(&mDivisionsMarkerViewport);

    mScrollLayout.addWidget(&mTracksWidget);
    mScrollLayout.addWidget(&mClipsGridViewport);
}

void Timeline::setupCallbacks()
{
    connect(mProject.get(), &Audio::Project::trackAdded, [=](int index)
    {
        auto *track = new Track(mProject->getTrackByIndex(index));
        mTracks.insert(index, track);
        mTracksLayout.insertWidget(index, track);
    });

    connect(mProject.get(), &Audio::Project::trackRemoved, [=](int index)
    {
        mTracksLayout.removeWidget(mTracks.at(index));
        mTracks.at(index)->deleteLater();
        mTracks.remove(index);
        mTimelineProperties->getCurrentSelection()->setSelectionType(SelectionManager::NoSelection);
    });

    connect(mTimelineProperties.get(), &TimelineProperties::zoomLevelChanged, [=]()
    {
        mDivisionsMarker->refresh(mClipsGrid->getDivision(), mTimelineProperties->getPixelsPerBeatAmount());
    });
}

void Timeline::setNewScrollPosition(int scrollPosInPixels)
{
    int newX = -scrollPosInPixels;
    int newWidth = mClipsGridViewport.width() + scrollPosInPixels;

    mClipsGrid->setGeometry(newX, 0, newWidth, mClipsGrid->height());
    mDivisionsMarker->setGeometry(newX, 0, newWidth, mDivisionsMarker->height());
}

int Timeline::getClipsGridWidth() const
{
    return mVerticalScrollView.width() - 151;
}

void Timeline::resizeEvent(QResizeEvent *)
{
    mClipsGrid->setGeometry(mClipsGrid->x(), 0, mClipsGridViewport.width(), mClipsGridViewport.height());
    mDivisionsMarker
        ->setGeometry(mDivisionsMarker->x(), 0, mDivisionsMarkerViewport.width(), mDivisionsMarkerViewport.height());
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
    bool trackClicked = false;
    for (const auto &track: mTracks) {
        if (track->geometry().contains(event->pos())) {
            mTimelineProperties->getCurrentSelection()->handleMousePressEvent(track.get(), event);
            trackClicked = true;
        }
    }

    if (!trackClicked) {
        mTimelineProperties->getCurrentSelection()->setSelectionType(SelectionManager::NoSelection);
        return;
    }

    if (event->buttons().testFlag(Qt::RightButton)) {
        QMenu menu(this);

        auto *deleteAction = new QAction("Delete", this);
        menu.addAction(deleteAction);

        if (mProject->canRemoveTracks((int) mTimelineProperties->getCurrentSelection()->getSelectedObjects().size())) {
            connect(deleteAction, &QAction::triggered, [this]()
            {
                for (const auto &track: mTimelineProperties->getCurrentSelection()->getSelectedObjects()) {
                    ((Track *) track.get())->deleteTrack();
                }
            });
        }
        else {
            deleteAction->setEnabled(false);
        }

        menu.exec(event->globalPosition().toPoint());
    }
}

void Timeline::mouseReleaseEvent(QMouseEvent *event)
{
    mTimelineProperties->getCurrentSelection()->handleMouseReleaseEvent();
}

}

#include "clipsgrid.h"

namespace Graphics
{

ClipsGrid::ClipsGrid(const QSharedPointer<Audio::Project> &project,
                     const QSharedPointer<TimelineProperties> &timelineProperties,
                     QWidget *parent)
    : QWidget(parent), mProject(project), mTimelineProperties(timelineProperties)
{
    mPositionBarWidget.setParent(this);
    mSelectionOverlay.setParent(this);

    mThumbnailManager = QSharedPointer<ThumbnailManager>::create();

    setupCallbacks();
}

void ClipsGrid::setupCallbacks()
{
    connect(mProject->getProjectProperties().get(), &Audio::ProjectProperties::bpmChanged, [=]()
    {
        updateClipsGeometry();
    });

    connect(mProject->getProjectProperties().get(), &Audio::ProjectProperties::totalLengthChanged, [=]()
    {
        updateMinimumSize();
    });

    connect(mTimelineProperties.get(), &TimelineProperties::pixelsPerBeatAmountChanged, [=]()
    {
        updateClipsGeometry();
    });

    connect(mTimelineProperties.get(), &TimelineProperties::currentSelectionChanged, [=]()
    {
        updateSelectionOverlay();
    });

    connect(mProject.get(), &Audio::Project::trackAdded, [=](int trackIndex)
    {
        update();
    });

    connect(mProject.get(), &Audio::Project::trackRemoved, [=](int trackIndex)
    {
        updateClipsGeometry();
    });

    connect(mProject.get(), &Audio::Project::clipAdded, [=](int clipID)
    {
        QPointer<Clip> clipUi = new Clip(mThumbnailManager, this);
        clipUi->show();

        auto newClip = mProject->getClips().at(clipID);
        clipUi->setClip(newClip);
        setupClipCallbacks(newClip.get());

        mClips.insert(clipID, clipUi);

        updateClipsGeometry();
    });

    connect(mProject.get(), &Audio::Project::clipRemoved, [=](int clipID)
    {
        mTimelineProperties->getCurrentSelection()->setSelectionType(SelectionManager::NoSelection);

        mClips.at(clipID)->deleteLater();
        mClips.remove(clipID);

        updateClipsGeometry();
    });

    connect(&mPositionBarTimer, &QTimer::timeout, this, &ClipsGrid::drawPositionBar);
    mPositionBarTimer.start(100);
}

void ClipsGrid::setupClipCallbacks(const QPointer<Audio::Clip> &clip)
{
    connect(clip->getClipProperties().get(), &Audio::ClipProperties::clipMoved, [=]()
    {
        updateClipsGeometry();
    });
    connect(clip->getClipProperties().get(), &Audio::ClipProperties::startOffsetChanged, [=]()
    {
        updateClipsGeometry();
    });
    connect(clip->getClipProperties().get(), &Audio::ClipProperties::endOffsetChanged, [=]()
    {
        updateClipsGeometry();
    });
}

void ClipsGrid::updateClipsGeometry()
{
    for (const auto &clipUi: mClips) {
        auto clipProperties = clipUi->getClip()->getClipProperties();

        int clipPosition = samplesToPixels(clipProperties->getPositionInSamples() + clipProperties->getStartOffset());
        int clipLength = samplesToPixels(clipProperties->getEndOffset() - clipProperties->getStartOffset());

        clipUi->setGeometry(clipPosition,
                            clipProperties->getParentTrack()->getTrackProperties()->getIndex()
                                * (DEFAULT_TRACK_HEIGHT + 1),
                            clipLength,
                            DEFAULT_TRACK_HEIGHT);
    }

    mPositionBarWidget.raise();
    mSelectionOverlay.raise();

    updateMinimumSize();
}

void ClipsGrid::updateSelectionOverlay()
{
    SelectionManager::SelectionArea area = mTimelineProperties->getCurrentSelection()->getSelectedArea();

    int x = samplesToPixels(area.startSample);
    int w = samplesToPixels(area.nbSamples);
    int y, h;

    if (area.nbTracks > 0) {
        y = area.startTrackIndex * (DEFAULT_TRACK_HEIGHT + 1);
        h = area.nbTracks * (DEFAULT_TRACK_HEIGHT + 1);
    }
    else {
        y = (area.startTrackIndex + 1) * (DEFAULT_TRACK_HEIGHT + 1);
        h = area.nbTracks * (DEFAULT_TRACK_HEIGHT + 1);
    }

    mSelectionOverlay.areaChanged(QRect(x, y, w, h));
    mSelectionOverlay.raise();
}

void ClipsGrid::updateMinimumSize()
{
    setMinimumSize(samplesToPixels((int) mProject->getTotalLength()),
                   150 * ((int) mProject->getTracks().size() + 1));

    emit sizeChanged();
}

double ClipsGrid::getDivision() const
{
    return Utils::calculateDivision(mTimelineProperties->getPixelsPerBeatAmount());
}

juce::int64 ClipsGrid::roundPosition(juce::int64 positionInSamples, bool forceRoundingToPriorPosition) const
{
    return Utils::roundPosition(positionInSamples, mTimelineProperties->getPixelsPerBeatAmount(),
                                mProject->getProjectProperties()->getBpm(),
                                forceRoundingToPriorPosition);
}

int ClipsGrid::samplesToPixels(juce::int64 samples) const
{
    return Utils::samplesToPixels(samples,
                                  mTimelineProperties->getPixelsPerBeatAmount(),
                                  mProject->getProjectProperties()->getBpm());
}

juce::int64 ClipsGrid::pixelsToSamples(int pixels) const
{
    return Utils::pixelsToSamples(pixels,
                                  mTimelineProperties->getPixelsPerBeatAmount(),
                                  mProject->getProjectProperties()->getBpm());
}

void ClipsGrid::drawPositionBar()
{
    mPositionBarWidget.barPositionChanged(samplesToPixels((int) mProject->getNextReadPosition()));
}

void ClipsGrid::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor("#E4E4E7")));

    int pixelsInDivision = int(mTimelineProperties->getPixelsPerBeatAmount() * getDivision());
    for (int i = 0; i * pixelsInDivision < geometry().width(); i++) {
        p.drawLine(i * pixelsInDivision, 0, i * pixelsInDivision, geometry().height());
    }

    for (int i = 0; i < (int) mProject->getTracks().size() + 1; i++) {
        p.drawLine(0, i * (DEFAULT_TRACK_HEIGHT + 1), geometry().width(), i * (DEFAULT_TRACK_HEIGHT + 1));
    }

    p.drawLine(0, geometry().height() - 1, geometry().width(), geometry().height() - 1);
}

void ClipsGrid::resizeEvent(QResizeEvent *)
{
    mPositionBarWidget.setGeometry(0, 0, width(), height());
    mSelectionOverlay.setGeometry(0, 0, width(), height());
    mPositionBarWidget.raise();
    mSelectionOverlay.raise();

    update();
}

void ClipsGrid::mousePressEvent(QMouseEvent *event)
{
    clickPosition = event->pos();

    if (event->buttons().testFlag(Qt::LeftButton))
        mProject->setNextReadPosition(roundPosition(pixelsToSamples(event->pos().x())));

    // selection system
    bool rightClick = event->buttons().testFlag(Qt::RightButton);
    bool clipClicked = false;

    bool selectionAreaClicked = mSelectionOverlay.geometry().contains(event->pos());
    if (mTimelineProperties->getCurrentSelection()->getSelectionType() != SelectionManager::AreaSelected)
        selectionAreaClicked = false;

    if (!selectionAreaClicked) {
        for (const auto &clip: mClips) {
            if (clip->geometry().contains(event->pos())) {
                mTimelineProperties->getCurrentSelection()->handleMousePressEvent(clip, event);
                clipClicked = true;
            }
        }
    }

    // if nothing have been selected and selection area wasn't right-clicked
    if (!clipClicked && !(selectionAreaClicked && rightClick))
        mTimelineProperties->getCurrentSelection()->setSelectionType(SelectionManager::NoSelection);

    if (rightClick) {
        if (selectionAreaClicked) {
            QMenu menu(this);

            auto *deleteAction = new QAction("Delete", this);
            menu.addAction(deleteAction);

            connect(deleteAction, &QAction::triggered, [this]()
            {
                auto area = mTimelineProperties->getCurrentSelection()->getSelectedArea();

                int startTrackID = area.startTrackIndex;
                int nbTracks = area.nbTracks;
                juce::int64 startSample = area.startSample;
                juce::int64 nbSamples = area.nbSamples;

                mProject->removeArea(startTrackID, nbTracks, startSample, nbSamples);
            });

            menu.exec(event->globalPosition().toPoint());
        }
        else if (clipClicked) {
            QMenu menu(this);

            auto *deleteAction = new QAction("Delete", this);
            menu.addAction(deleteAction);

            connect(deleteAction, &QAction::triggered, [this]()
            {
                for (const auto &clip: mTimelineProperties->getCurrentSelection()->getSelectedObjects()) {
                    mProject->removeClip(((Clip *) clip.get())->getClip());
                }
            });

            menu.exec(event->globalPosition().toPoint());
        }
    }
}

void ClipsGrid::mouseReleaseEvent(QMouseEvent *)
{
    if (mMovingClip) {
        mMovingClip->getClip()->getClipProperties()->setPositionInSamples(
            pixelsToSamples(mMovingClip->x()) - mMovingClip->getClip()->getClipProperties()->getStartOffset());
        mMovingClip = nullptr;
    }

    mTimelineProperties->getCurrentSelection()->handleMouseReleaseEvent();
}

void ClipsGrid::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::LeftButton)) {
        if (event->pos().y() > (DEFAULT_TRACK_HEIGHT + 1) * (int) mProject->getTracks().size() - 1)
            return;

        // move clip if needed
        if (mMovingClip == nullptr) {
            for (const auto &clip: mClips) {
                if ((clip->shouldMoveClip(clickPosition) && clip->geometry().contains(clickPosition))) {
                    mMovingClip = clip.get();
                }
            }
        }

        if (mMovingClip != nullptr) {
            juce::int64 clipPosition = mMovingClip->getClip()->getClipProperties()->getPositionInSamples()
                + mMovingClip->getClip()->getClipProperties()->getStartOffset();

            juce::int64 newPositionInSamples =
                roundPosition(
                    clipPosition + (juce::int64) pixelsToSamples((int) event->position().x() - clickPosition.x()));

            mMovingClip->setGeometry(samplesToPixels(newPositionInSamples),
                                     mMovingClip->y(),
                                     mMovingClip->width(),
                                     mMovingClip->height());
            return;
        }

        // else set selection area
        int trackIndex = event->pos().y() / (DEFAULT_TRACK_HEIGHT + 1);
        juce::int64 samples = roundPosition(pixelsToSamples(event->pos().x()));

        SelectionManager::SelectionArea area = mTimelineProperties->getCurrentSelection()->getSelectedArea();
        if (mTimelineProperties->getCurrentSelection()->getSelectionType() != SelectionManager::AreaSelected) {
            mTimelineProperties->getCurrentSelection()->setSelectionType(SelectionManager::AreaSelected);
            area.startTrackIndex = trackIndex;
            area.startSample = samples;
        }

        if (trackIndex >= area.startTrackIndex)
            area.nbTracks = trackIndex - area.startTrackIndex + 1;
        else
            area.nbTracks = trackIndex - area.startTrackIndex - 1;

        area.nbSamples = samples - area.startSample;

        mTimelineProperties->getCurrentSelection()->setSelectedArea(area);
    }
}

void ClipsGrid::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int trackClickedIndex = event->pos().y() / (DEFAULT_TRACK_HEIGHT + 1);
        auto track = mProject->getTrackByIndex(trackClickedIndex);
        if (track->getType() == Audio::Track::ANY_TRACK || track->getType() == Audio::Track::MIDI_TRACK) {
            int clipID = mProject->createMIDIClip(track);

            juce::int64 positionInSamples = pixelsToSamples(event->pos().x());
            positionInSamples = roundPosition(positionInSamples, true);

            auto properties = mProject->getClips().at(clipID)->getClipProperties();
            properties->setPositionInSamples(positionInSamples);

            juce::int64
                samplesInDivision = pixelsToSamples(int(mTimelineProperties->getPixelsPerBeatAmount() * getDivision()));
            properties->setLengthInSamples(samplesInDivision);
            properties->setEndOffset(samplesInDivision);
        }
    }
}

} // namespace Graphics

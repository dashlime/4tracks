#include "timelineproperties.h"

namespace Graphics
{

TimelineProperties::TimelineProperties(const QSharedPointer<SelectionManager> &currentSelection)
{
    setCurrentSelection(currentSelection);
}

void TimelineProperties::setCurrentSelection(const QSharedPointer<SelectionManager> &currentSelection)
{
    mCurrentSelection = currentSelection;

    connect(mCurrentSelection.get(), &SelectionManager::selectionChanged, [=]()
    {
        emit currentSelectionChanged();
    });
}

const QSharedPointer<SelectionManager> &TimelineProperties::getCurrentSelection() const
{
    return mCurrentSelection;
}

void TimelineProperties::setZoomLevel(double zoomLevel)
{
    mZoomLevel = zoomLevel;
    mPixelsPerBeat = mZoomLevel * DEFAULT_PIXEL_PER_BEAT_AMOUNT;

    emit zoomLevelChanged();
    emit pixelsPerBeatAmountChanged();
}

double TimelineProperties::getZoomLevel() const
{
    return mZoomLevel;
}

double TimelineProperties::getPixelsPerBeatAmount() const
{
    return mPixelsPerBeat;
}

}
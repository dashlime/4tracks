#include "timelineproperties.h"

namespace Graphics
{

TimelineProperties::TimelineProperties(const QSharedPointer<SelectionManager> &currentSelection, double basePixelsPerBeatAmount)
    : mBasePixelsPerBeatAmount(basePixelsPerBeatAmount)
{
    updatePixelsPerBeatAmount();

    setCurrentSelection(currentSelection);
}

void TimelineProperties::setBasePixelsPerBeatAmount(double newBasePixelsPerBeatAmount)
{
    mBasePixelsPerBeatAmount = newBasePixelsPerBeatAmount;
    updatePixelsPerBeatAmount();
}

void TimelineProperties::updatePixelsPerBeatAmount()
{
    mPixelsPerBeat = mZoomLevel * mBasePixelsPerBeatAmount;

    emit pixelsPerBeatAmountChanged();
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
    updatePixelsPerBeatAmount();

    emit zoomLevelChanged();
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
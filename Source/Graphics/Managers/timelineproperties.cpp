#include "timelineproperties.h"

namespace Graphics
{

TimelineProperties::TimelineProperties(const QSharedPointer<Selection> &currentSelection)
{
    setCurrentSelection(currentSelection);
}

void TimelineProperties::setCurrentSelection(const QSharedPointer<Selection> &currentSelection)
{
    mCurrentSelection = currentSelection;
    mCurrentSelection->setSelectionCallback(this);
}

const QSharedPointer<Selection> &TimelineProperties::getCurrentSelection() const
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

void TimelineProperties::selectionChanged()
{
    emit currentSelectionChanged();
}

}
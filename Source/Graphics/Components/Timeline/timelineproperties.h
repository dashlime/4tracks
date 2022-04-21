#ifndef TIMELINEPROPERTIES_H
#define TIMELINEPROPERTIES_H

#include <QObject>

#include "Graphics/Managers/selectionmanager.h"
#include "Constants.h"

namespace Graphics
{

class TimelineProperties: public QObject
{
Q_OBJECT

public:
    explicit TimelineProperties(const QSharedPointer<SelectionManager> &currentSelection, double basePixelsPerBeatAmount = DEFAULT_PIXEL_PER_BEAT_AMOUNT);

    void setBasePixelsPerBeatAmount(double newBasePixelsPerBeatAmount);
    void updatePixelsPerBeatAmount();

    void setCurrentSelection(const QSharedPointer<SelectionManager> &currentSelection);
    [[nodiscard]] const QSharedPointer<SelectionManager> &getCurrentSelection() const;

    void setZoomLevel(double zoomLevel);
    [[nodiscard]] double getZoomLevel() const;

    [[nodiscard]] double getPixelsPerBeatAmount() const;

signals:
    void currentSelectionChanged();
    void zoomLevelChanged();
    void pixelsPerBeatAmountChanged();

private:
    QSharedPointer<SelectionManager> mCurrentSelection;

    double mBasePixelsPerBeatAmount;
    double mZoomLevel = 1.f;
    double mPixelsPerBeat;
};

}

#endif //TIMELINEPROPERTIES_H

#ifndef TIMELINEPROPERTIES_H
#define TIMELINEPROPERTIES_H

#include <QObject>

#include "selection.h"
#include "Constants.h"

namespace Graphics
{

class TimelineProperties: public QObject
{
Q_OBJECT

public:
    explicit TimelineProperties(const QSharedPointer<Selection> &currentSelection);

    void setCurrentSelection(const QSharedPointer<Selection> &currentSelection);
    [[nodiscard]] const QSharedPointer<Selection> &getCurrentSelection() const;

    void setZoomLevel(double zoomLevel);
    [[nodiscard]] double getZoomLevel() const;

    [[nodiscard]] double getPixelsPerBeatAmount() const;

signals:
    void currentSelectionChanged();
    void zoomLevelChanged();
    void pixelsPerBeatAmountChanged();

private:
    QSharedPointer<Selection> mCurrentSelection;

    double mZoomLevel = 1.f;
    double mPixelsPerBeat = mZoomLevel * DEFAULT_PIXEL_PER_BEAT_AMOUNT;
};

}

#endif //TIMELINEPROPERTIES_H

#ifndef TIMELINEPROPERTIES_H
#define TIMELINEPROPERTIES_H

#include <QObject>

#include "selection.h"
#include "Constants.h"

namespace Graphics
{

class TimelineProperties: public QObject, public Selection::Callback
{
Q_OBJECT

public:
    explicit TimelineProperties(const QSharedPointer<Selection> &currentSelection);

    void setCurrentSelection(const QSharedPointer<Selection> &currentSelection);
    const QSharedPointer<Selection> &getCurrentSelection() const;

    void setZoomLevel(double zoomLevel);
    double getZoomLevel() const;

    double getPixelsPerBeatAmount() const;

    void selectionChanged() override;

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

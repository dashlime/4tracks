#include "selectionoverlay.h"

namespace Graphics
{

SelectionOverlay::SelectionOverlay(bool filled, QWidget *parent)
    : QWidget(parent), mFilled(filled)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void SelectionOverlay::areaChanged(QRect selectionArea)
{
    mSelectionArea = selectionArea;
    update();
}

void SelectionOverlay::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    if (mFilled) {
        QColor selectionColor("#3B82F6");
        selectionColor.setAlpha(127);

        p.fillRect(mSelectionArea, selectionColor);
    } else {
        p.setPen(QColorConstants::Black);
        p.drawRect(mSelectionArea);
    }
}

bool SelectionOverlay::areaContains(QPoint point) const
{
    point -= pos();

    if (mSelectionArea.contains(point))
        return true;

    return false;
}

} // namespace Graphics

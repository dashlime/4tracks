#include "positionbaroverlay.h"

namespace Graphics
{

PositionBarOverlay::PositionBarOverlay(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void PositionBarOverlay::barPositionChanged(int newPosition)
{
    mPosition = newPosition;
    update();
}

void PositionBarOverlay::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.drawLine(mPosition, 0, mPosition, height());

}

} // namespace Graphics

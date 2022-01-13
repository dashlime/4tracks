#include "divisionsmarker.h"

namespace Graphics {

DivisionsMarker::DivisionsMarker(double division, double pixelsPerBeat, QWidget *parent) : QWidget(parent), mDivision(division), mPixelsPerBeat(pixelsPerBeat)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFixedHeight(10);
}

void DivisionsMarker::refresh(double newDivision, double pixelsPerBeat)
{
    mDivision = newDivision;
    mPixelsPerBeat = pixelsPerBeat;
    update();
}

void DivisionsMarker::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(QColorConstants::Black);

    if (mPixelsPerBeat == 0 || mDivision == 0)
        return;

    QFont font = p.font();
    font.setPixelSize(10);
    p.setFont(font);

    for (int i = 0; i*mPixelsPerBeat*mDivision < width(); i+=4)
    {
        p.drawText(i*mPixelsPerBeat*mDivision, 10, QString::number(mDivision * i / 4));
    }
}

} // namespace Graphics

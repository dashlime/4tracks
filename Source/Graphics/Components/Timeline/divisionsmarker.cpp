#include "divisionsmarker.h"

namespace Graphics
{

DivisionsMarker::DivisionsMarker(double division, double pixelsPerBeat, QWidget *parent)
    : QWidget(parent), mDivision(division), mPixelsPerBeat(pixelsPerBeat)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFixedHeight(10);
}

QString DivisionsMarker::beatToString(double beat)
{
    if (beat == 0)
        return "1";

    beat /= 4;

    QString result = "";

    double currentDivision = 1;
    while (beat != 0) {
        int beatNumber = (int) floor(beat / currentDivision) + 1;
        beat = fmod(beat, currentDivision);
        result += QString::number(beatNumber);
        result += ".";

        currentDivision /= 4;
    }

    result.chop(1);

    return result;
}

void DivisionsMarker::refresh(double newDivision, double pixelsPerBeat)
{
    mDivision = newDivision;
    mPixelsPerBeat = pixelsPerBeat;
    update();
}

void DivisionsMarker::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QColorConstants::Black);

    if (mPixelsPerBeat == 0 || mDivision == 0)
        return;

    QFont font = p.font();
    font.setPixelSize(10);
    p.setFont(font);

    for (int i = 0; i * mPixelsPerBeat * mDivision < width(); i += 4) {
        p.drawText(int(i * mPixelsPerBeat * mDivision), 10, beatToString(mDivision * i));
    }
}

} // namespace Graphics

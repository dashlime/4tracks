#include "notevelocity.h"

namespace Graphics
{

NoteVelocity::NoteVelocity(const QSharedPointer<Audio::MidiNote>& correspondingMidiNote, QWidget *parent) : QWidget(parent), mNote(correspondingMidiNote)
{
    setMinimumSize(10, 10);
}

QSharedPointer<Audio::MidiNote> NoteVelocity::getCorrespondingMidiNote() const
{
    return mNote;
}

void NoteVelocity::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QPen pen = QColor(239, 68, 68);
    pen.setWidth(2);
    p.setPen(pen);


    p.drawEllipse(1, 1, 8, 8);
    p.drawLine(10, 5, QWidget::width(), 5);
}

}
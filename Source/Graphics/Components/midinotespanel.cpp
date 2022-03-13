#include "midinotespanel.h"

namespace Graphics
{

MidiNotesPanel::MidiNotesPanel(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent) : QWidget(parent), mClip(clip)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(90);
}

void MidiNotesPanel::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor("#E4E4E7")));

    int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    for (int i = 0; i < nbNotesToDraw; i++) {
        p.fillRect(70, i * 20, 20, 20, mClip->currentScaleContains(nbNotesToDraw - i) ? QColorConstants::White : QColorConstants::Black);

        p.drawLine(0, i * 20, width(), i * 20);
    }
    p.drawLine(70, 0, 70, height());
    p.drawLine(90, 0, 90, height());

    p.setPen(QColorConstants::Black);

    auto font = QApplication::font();
    font.setPixelSize(12);
    font.setWeight(QFont::Weight::Bold);
    p.setFont(font);

    for (int i = 0; i < nbNotesToDraw; i++) {
        p.drawText(QRect(3, i * 20, 65, 20), Qt::AlignVCenter | Qt::AlignLeft,mClip->midiNoteName(nbNotesToDraw - i));
    }
}

}
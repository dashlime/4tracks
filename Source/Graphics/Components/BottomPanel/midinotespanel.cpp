#include "midinotespanel.h"

namespace Graphics
{

MidiNotesPanel::MidiNotesPanel(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent)
    : QWidget(parent), mClip(clip)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    setFixedWidth(DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH);
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
        p.fillRect(DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH - 20,
                   i * DEFAULT_NOTE_HEIGHT,
                   DEFAULT_NOTE_HEIGHT,
                   DEFAULT_NOTE_HEIGHT,
                   mClip->currentScaleContains(nbNotesToDraw - i) ? QColorConstants::White : QColorConstants::Black);

        p.drawLine(0, i * DEFAULT_NOTE_HEIGHT, width(), i * DEFAULT_NOTE_HEIGHT);
    }
    p.drawLine(DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH - 20, 0, DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH - 20, height());
    p.drawLine(DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH, 0, DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH, height());

    p.setPen(QColorConstants::Black);

    auto font = QApplication::font();
    font.setPixelSize(12);
    font.setWeight(QFont::Weight::Bold);
    p.setFont(font);

    for (int i = 0; i < nbNotesToDraw; i++) {
        p.drawText(QRect(3, i * DEFAULT_NOTE_HEIGHT, 65, DEFAULT_NOTE_HEIGHT), Qt::AlignVCenter | Qt::AlignLeft, mClip->midiNoteName(nbNotesToDraw - i));
    }
}

}
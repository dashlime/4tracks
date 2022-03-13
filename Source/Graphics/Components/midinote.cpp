#include "midinote.h"

namespace Graphics
{

MidiNote::MidiNote(const juce::MidiMessage &noteOnEvent,
                   const QSharedPointer<Audio::MidiClip>& clip,
                   QWidget *parent)
    : QWidget(parent), mClip(clip)
{
    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(2, 0, 0, 0);

    mNoteLabel.setText(mClip->midiNoteName(noteOnEvent.getNoteNumber()));
    auto font = QApplication::font();
    font.setPixelSize(10);

    mNoteLabel.setFont(font);
    mNoteLabel.setStyleSheet("color: white;");

    layout()->addWidget(&mNoteLabel);
}

void MidiNote::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}
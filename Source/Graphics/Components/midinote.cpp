#include "midinote.h"

namespace Graphics
{

MidiNote::MidiNote(const QSharedPointer<Audio::MidiNote> &note,
                   const QSharedPointer<Audio::MidiClip>& clip,
                   QWidget *parent)
    : Selection::SelectableObject(parent), mClip(clip), mNote(note)
{
    setLayout(new QHBoxLayout());
    layout()->setContentsMargins(2, 0, 0, 0);

    mNoteLabel.setText(mClip->midiNoteName(note->getMidiMessage().getNoteNumber()));
    auto font = QApplication::font();
    font.setPixelSize(10);

    mNoteLabel.setFont(font);
    mNoteLabel.setStyleSheet("color: white;");

    layout()->addWidget(&mNoteLabel);

    setSelectedState(false);

    mNoteLabel.setMouseTracking(true);
}

QSharedPointer<Audio::MidiNote> MidiNote::getMidiNote() const
{
    return mNote;
}

void MidiNote::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MidiNote::setSelectedState(bool isSelected)
{
    if (isSelected) {
        setStyleSheet("background-color: rgba(239, 68, 68, 50%);");
    } else {
        setStyleSheet("background-color: rgba(239, 68, 68, 100%);");
    }
}

Selection::SelectableObject::Type MidiNote::getType() const
{
    return Selection::SelectableObject::MidiNote;
}

}
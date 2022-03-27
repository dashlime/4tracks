#include "mididata.h"

namespace Audio
{

MidiData::MidiData()
{}

void MidiData::addNote(const QSharedPointer<MidiNote> &note)
{
    mNotes.push_back(note);

    connect(note.get(), &MidiNote::dataChanged, [=]() {
        emit noteDataChanged(note);
    });

    emit noteAdded(note);
}

void MidiData::removeNote(const QSharedPointer<MidiNote> &note)
{
    mNotes.remove(mNotes.indexOf(note));
    mNotes.remove(mNotes.indexOf(note->getNoteOffObject()));

    note->disconnect();

    emit noteRemoved(note);
}

QVector<QSharedPointer<MidiNote>> MidiData::getMidiNotes() const
{
    return mNotes;
}

}

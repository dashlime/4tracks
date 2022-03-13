#include "mididata.h"

namespace Audio
{

/* -------------------------------- MIDI NOTE PART -------------------------------- */

MidiNote::MidiNote(juce::int64 positionInSamples, const juce::MidiMessage &midiMessage)
    : mPositionInSamples(positionInSamples), mMidiMessage(midiMessage)
{}

juce::int64 MidiNote::getPositionInSamples() const
{
    return mPositionInSamples;
}

void MidiNote::setPositionInSamples(juce::int64 positionInSamples)
{
    mPositionInSamples = positionInSamples;
    emit dataChanged();
}

const juce::MidiMessage &MidiNote::getMidiMessage() const
{
    return mMidiMessage;
}

void MidiNote::setMidiMessage(const juce::MidiMessage &midiMessage)
{
    mMidiMessage = midiMessage;
    emit dataChanged();
}

const QSharedPointer<MidiNote> &MidiNote::getNoteOffObject() const
{
    return mNoteOffObject;
}

void MidiNote::setNoteOffObject(const QSharedPointer<MidiNote> &noteOffObject)
{
    mNoteOffObject = noteOffObject;
    emit dataChanged();
}

/* -------------------------------- MIDI DATA PART -------------------------------- */

MidiData::MidiData()
{}

void MidiData::addNote(const QSharedPointer<MidiNote> &note)
{
    mNotes.push_back(note);

    connect(note.get(), &MidiNote::dataChanged, [=]()
    {
        emit dataChanged();
    });

    emit dataChanged();
}

QVector<QSharedPointer<MidiNote>> MidiData::getMidiNotes() const
{
    return mNotes;
}

}

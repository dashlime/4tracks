#include "midiclip.h"

namespace Audio
{

MidiClip::MidiClip(Track *parentTrack)
    : Clip("Midi Clip", parentTrack)
{
    mMidiData = QSharedPointer<MidiData>::create();

    connect(mMidiData.get(), &MidiData::noteAdded, [=]()
    {
        emit dataChanged();
    });
    connect(mMidiData.get(), &MidiData::noteRemoved, [=]()
    {
        emit dataChanged();
    });
    connect(mMidiData.get(), &MidiData::noteDataChanged, [=]()
    {
        emit dataChanged();
    });
}

QSharedPointer<MidiData> MidiClip::getMidiData() const
{
    return mMidiData;
}

bool MidiClip::currentScaleContains(int midiNoteNumber) const
{
    int note = midiNoteNumber % NOTES_IN_OCTAVE;

    if (MAJOR_SCALE_SCHEME[note])
        return true;

    return false;
}

QString MidiClip::midiNoteName(int midiNoteNumber) const
{
    QString note = juce::MidiMessage::getMidiNoteName(midiNoteNumber, true, true, 4).toStdString().c_str();
    return note;
}

Clip::Type MidiClip::getType() const
{
    return Clip::MIDI_CLIP;
}

void MidiClip::nextReadPositionChanged()
{}

void MidiClip::removeArea(int startNote, int nbNotes, juce::int64 relativeStartSample, juce::int64 nbSamples)
{
    if (nbSamples < 0) {
        relativeStartSample = relativeStartSample + nbSamples;
        nbSamples = -nbSamples;
    }

    if (nbNotes < 0) {
        // add 1 because note indexes starts with 0 and not with 1 :
        //    ex: startNote(1) + nbNotes(-2) + 1 = 0
        startNote = startNote + nbNotes + 1;
        nbNotes = -nbNotes;
    }

    for (int i = startNote; i < startNote + nbNotes; i++) {
        for (auto &note: mMidiData->getMidiNotes()) {
            if (!note->getNoteOffObject().isNull()) {
                juce::int64 notePosition = note->getPositionInSamples();
                juce::int64 noteEndPosition = note->getNoteOffObject()->getPositionInSamples();

                if (relativeStartSample <= notePosition && relativeStartSample + nbSamples >= noteEndPosition) {
                    mMidiData->removeNote(note);
                }
                else if (relativeStartSample > notePosition && relativeStartSample + nbSamples < noteEndPosition) {
                    auto newNote = QSharedPointer<MidiNote>::create(note);
                    newNote->setPositionInSamples(relativeStartSample + nbSamples);
                    mMidiData->addNote(newNote);

                    note->getNoteOffObject()->setPositionInSamples(relativeStartSample);
                }
                else {
                    if (relativeStartSample > notePosition && relativeStartSample < noteEndPosition) {
                        note->getNoteOffObject()->setPositionInSamples(relativeStartSample);
                    }
                    else if (relativeStartSample + nbSamples > notePosition
                        && relativeStartSample + nbSamples < noteEndPosition) {
                        note->setPositionInSamples(relativeStartSample + nbSamples);
                    }
                }
            }
        }
    }
}

}

#ifndef MIDIDATA_H
#define MIDIDATA_H

#include <QtWidgets>
#include "Audio/JuceIncludes.h"

namespace Audio
{

class MidiNote: public QObject
{
Q_OBJECT

public:
    MidiNote(juce::int64 positionInSamples, const juce::MidiMessage &midiMessage);

    [[nodiscard]] juce::int64 getPositionInSamples() const;
    void setPositionInSamples(juce::int64 positionInSamples);

    [[nodiscard]] const juce::MidiMessage &getMidiMessage() const;
    void setMidiMessage(const juce::MidiMessage &midiMessage);

    [[nodiscard]] const QSharedPointer<MidiNote> &getNoteOffObject() const;
    void setNoteOffObject(const QSharedPointer<MidiNote> &noteOffObject);

signals:
    void dataChanged();

private:
    juce::int64 mPositionInSamples = 0;
    juce::MidiMessage mMidiMessage;
    QSharedPointer<MidiNote> mNoteOffObject;
};

class MidiData: public QObject
{
    Q_OBJECT

public:

    MidiData();

    void addNote(const QSharedPointer<MidiNote>& note);
    void removeNote(const QSharedPointer<MidiNote>& note);

    [[nodiscard]] QVector<QSharedPointer<MidiNote>> getMidiNotes() const;

signals:
    void noteAdded(const QSharedPointer<MidiNote>& note);
    void noteRemoved(const QSharedPointer<MidiNote>& note);

private:
    QVector<QSharedPointer<MidiNote>> mNotes;
};

}

#endif //MIDIDATA_H

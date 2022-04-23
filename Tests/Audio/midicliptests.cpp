#include "audioenginetests.h"

TEST_F(MidiClipTests, CurrentScaleContains)
{
    // test with C-1
    EXPECT_TRUE(mMidiClip->currentScaleContains(0));
    // test with D2
    EXPECT_TRUE(mMidiClip->currentScaleContains(NOTES_IN_OCTAVE * 3 + 2));
    // test with D#2
    EXPECT_FALSE(mMidiClip->currentScaleContains(NOTES_IN_OCTAVE * 3 + 3));
}

TEST_F(MidiClipTests, GetMidiNoteName)
{
    // test with C-1
    EXPECT_TRUE(mMidiClip->midiNoteName(0) == "C-1");
    // test with D2
    EXPECT_TRUE(mMidiClip->midiNoteName(NOTES_IN_OCTAVE * 3 + 2) == "D2");
    // test with D#2
    EXPECT_TRUE(mMidiClip->midiNoteName(NOTES_IN_OCTAVE * 3 + 3) == "D#2");
}

TEST_F(MidiClipTests, AddAndRemoveNotes)
{
    auto noteOn = QSharedPointer<Audio::MidiNote>::create(0, juce::MidiMessage::noteOn(1, 10, 1.f));
    auto noteOff = QSharedPointer<Audio::MidiNote>::create(10, juce::MidiMessage::noteOff(1, 10));

    noteOn->setNoteOffObject(noteOff);

    mMidiClip->getMidiData()->addNote(noteOn);
    mMidiClip->getMidiData()->addNote(noteOff);

    EXPECT_EQ(mMidiClip->getMidiData()->getMidiNotes().size(), 2);

    mMidiClip->getMidiData()->removeNote(noteOn);

    EXPECT_EQ(mMidiClip->getMidiData()->getMidiNotes().size(), 0);
}

TEST_F(MidiClipTests, DuplicateNote)
{
    auto noteOn = QSharedPointer<Audio::MidiNote>::create(0, juce::MidiMessage::noteOn(1, 10, 1.f));
    auto noteOff = QSharedPointer<Audio::MidiNote>::create(10, juce::MidiMessage::noteOff(1, 10));

    noteOn->setNoteOffObject(noteOff);

    mMidiClip->getMidiData()->addNote(noteOn);
    mMidiClip->getMidiData()->addNote(noteOff);

    auto newNote = QSharedPointer<Audio::MidiNote>::create(noteOn);
    EXPECT_EQ(newNote->getPositionInSamples(), 0);
    EXPECT_EQ(newNote->getNoteOffObject()->getPositionInSamples(), 10);
    EXPECT_EQ(newNote->getMidiMessage().getNoteNumber(), 10);

    mMidiClip->getMidiData()->addNote(newNote);
    mMidiClip->getMidiData()->addNote(newNote->getNoteOffObject());

    EXPECT_EQ(mMidiClip->getMidiData()->getMidiNotes().size(), 4);
}

TEST_F(MidiClipTests, ChangeNotesParams)
{
    auto noteOn = QSharedPointer<Audio::MidiNote>::create(0, juce::MidiMessage::noteOn(1, 10, 1.f));
    mMidiClip->getMidiData()->addNote(noteOn);

    mParentProject->getProjectProperties()->updateSavedState(Audio::ProjectProperties::SAVED);
    noteOn->setPositionInSamples(10);
    EXPECT_EQ(mParentProject->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);

    mParentProject->getProjectProperties()->updateSavedState(Audio::ProjectProperties::SAVED);
    noteOn->setMidiMessage(juce::MidiMessage::noteOn(1, 20, 1.f));
    EXPECT_EQ(mParentProject->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);
}

TEST_F(MidiClipTests, PlayThroughMidiClip)
{
    mMidiClip->getClipProperties()->setLengthInSamples(10);
    mMidiClip->getClipProperties()->setEndOffset(10);

    mParentProject->play();

    auto *buffer = new juce::AudioBuffer<float>(2, 1024);
    auto channelInfo = juce::AudioSourceChannelInfo(buffer, 0, 1024);

    mParentProject->getNextAudioBlock(channelInfo);

    EXPECT_EQ(mParentProject->getNextReadPosition(), 1024);
    EXPECT_EQ(mParentTrack->getNextReadPosition(), 1024);

    delete buffer;
}
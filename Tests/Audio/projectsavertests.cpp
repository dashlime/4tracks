#include "audioenginetests.h"

TEST_F(AudioProjectSaverTests, SaveAndOpenProject)
{
    mProject->createTrack("track 1");
    mProject->createTrack("track 2");
    mProject->createAudioClip(mProject->getTrackByIndex(0), "");
    int midiClipID = mProject->createMIDIClip(mProject->getTrackByIndex(1));
    auto midiClip = qSharedPointerDynamicCast<Audio::MidiClip>(mProject->getClips().at(midiClipID));

    auto noteOn = QSharedPointer<Audio::MidiNote>::create(0, juce::MidiMessage::noteOn(1, 10, 1.f));
    auto noteOff = QSharedPointer<Audio::MidiNote>::create(20, juce::MidiMessage::noteOff(1, 10));

    noteOn->setNoteOffObject(noteOff);

    midiClip->getMidiData()->addNote(noteOn);
    midiClip->getMidiData()->addNote(noteOff);

    QDir projectDir = QDir::current().absolutePath() + "/project";
    mProjectSaver->saveToDirectory(projectDir);

    SetUp();

    mProjectSaver->openProject(projectDir.absolutePath() + "/project.4tpro");
    EXPECT_EQ(mProject->getProjectProperties()->getProjectName(), "project");
    EXPECT_EQ(mProject->getTracks().size(), 2);
    EXPECT_EQ(mProject->getClips().size(), 2);
    EXPECT_EQ(mProject->getTrackByIndex(0)->getTrackProperties()->getName(), "track 1");
    EXPECT_EQ(mProject->getTrackByIndex(1)->getTrackProperties()->getName(), "track 2");
    EXPECT_EQ(mProject->getTrackByIndex(0)->getClips().at(0)->getType(), Audio::Clip::AUDIO_CLIP);
    EXPECT_EQ(mProject->getTrackByIndex(1)->getClips().at(0)->getType(), Audio::Clip::MIDI_CLIP);

    midiClip = qSharedPointerDynamicCast<Audio::MidiClip>(mProject->getTrackByIndex(1)->getClips().at(0));
    EXPECT_EQ(midiClip->getMidiData()->getMidiNotes().at(0)->getPositionInSamples(), 0);
    EXPECT_EQ(midiClip->getMidiData()->getMidiNotes().at(0)->getMidiMessage().getNoteNumber(), 10);
    EXPECT_EQ(midiClip->getMidiData()->getMidiNotes().at(0)->getNoteOffObject()->getPositionInSamples(), 20);
}
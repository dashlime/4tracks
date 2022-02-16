#include "audioenginetests.h"

TEST_F(AudioProjectSaverTests, SaveAndOpenProject)
{
    mProject->createTrack("track 1");
    mProject->createTrack("track 2");
    mProject->createAudioClip(mProject->getTrackByIndex(0), "");
    mProject->createMIDIClip(mProject->getTrackByIndex(1));

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
}
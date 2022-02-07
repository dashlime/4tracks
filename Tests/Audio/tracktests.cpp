#include "Audio/Clip/midiclip.h"

#include "audioenginetests.h"

TEST_F(TrackTests, GetTypeWithDifferentClips)
{
    EXPECT_EQ(mTrackToTest->getType(), Audio::Track::ANY_TRACK);

    mProjectToTest->createAudioClip(mTrackToTest, "");
    EXPECT_EQ(mTrackToTest->getType(), Audio::Track::AUDIO_TRACK);

    SetUp();
    mProjectToTest->createMIDIClip(mTrackToTest);
    EXPECT_EQ(mTrackToTest->getType(), Audio::Track::MIDI_TRACK);
}

TEST_F(TrackTests, AddClip)
{
    auto clipToAdd = QSharedPointer<Audio::AudioClip>::create("", mTrackToTest.get());
    EXPECT_TRUE(mTrackToTest->addClip(clipToAdd));
    EXPECT_EQ(mTrackToTest->getClips().at(0).get(), clipToAdd.get());
}

TEST_F(TrackTests, AddClipWithDifferentType)
{
    mProjectToTest->createAudioClip(mTrackToTest, "");
    int result = mProjectToTest->createMIDIClip(mTrackToTest);

    EXPECT_EQ(result, -1);
    EXPECT_EQ(mTrackToTest->getClips().size(), 1);
}

TEST_F(TrackTests, RemoveClip)
{
    int clipID = mProjectToTest->createAudioClip(mTrackToTest, "");

    mTrackToTest->removeClip(mProjectToTest->getClips().at(clipID));
    EXPECT_EQ(mTrackToTest->getClips().size(), 0);
}
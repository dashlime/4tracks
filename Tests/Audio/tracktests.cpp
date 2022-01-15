#include <Audio/midiclip.h>

#include "audioenginetests.h"

TEST_F(TrackTests, GetTypeWithDifferentClips)
{
    EXPECT_EQ(mTrackToTest->getType(), Audio::Track::ANY_TRACK);

    mTrackToTest->addClip(std::make_shared<Audio::AudioClip>(mTrackToTest, ""));
    EXPECT_EQ(mTrackToTest->getType(), Audio::Track::AUDIO_TRACK);

    SetUp();
    mTrackToTest->addClip(std::make_shared<Audio::MidiClip>(mTrackToTest));
    EXPECT_EQ(mTrackToTest->getType(), Audio::Track::MIDI_TRACK);
}

TEST_F(TrackTests, AddClip)
{
    auto clipToAdd = std::make_shared<Audio::AudioClip>(mTrackToTest, "");
    EXPECT_TRUE(mTrackToTest->addClip(clipToAdd));
    EXPECT_EQ(mTrackToTest->getClips().at(0).get(), clipToAdd.get());
}

TEST_F(TrackTests, AddClipWithDifferentType)
{
    auto audioClip = std::make_shared<Audio::AudioClip>(mTrackToTest, "");
    mTrackToTest->addClip(audioClip);

    auto midiClip = std::make_shared<Audio::MidiClip>(mTrackToTest);
    EXPECT_FALSE(mTrackToTest->addClip(midiClip));
    EXPECT_EQ(mTrackToTest->getClips().size(), 1);
    EXPECT_NE(mTrackToTest->getClips().at(0).get(), midiClip.get());
}
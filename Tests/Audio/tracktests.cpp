#include "Audio/Clip/midiclip.h"

#include "audioenginetests.h"

TEST_F(TrackTests, GetTypeWithDifferentClips)
{
    EXPECT_EQ(mTrackToTest->getType(), Audio::Track::ANY_TRACK);

    mTrackToTest->addClip(QSharedPointer<Audio::AudioClip>::create("", mTrackToTest.get()));
    EXPECT_EQ(mTrackToTest->getType(), Audio::Track::AUDIO_TRACK);

    SetUp();
    mTrackToTest->addClip(QSharedPointer<Audio::MidiClip>::create(mTrackToTest.get()));
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
    auto audioClip = QSharedPointer<Audio::AudioClip>::create("", mTrackToTest.get());
    mTrackToTest->addClip(audioClip);

    auto midiClip = QSharedPointer<Audio::MidiClip>::create(mTrackToTest.get());
    EXPECT_FALSE(mTrackToTest->addClip(midiClip));
    EXPECT_EQ(mTrackToTest->getClips().size(), 1);
    EXPECT_NE(mTrackToTest->getClips().at(0).get(), midiClip.get());
}

TEST_F(TrackTests, RemoveClip)
{
    auto audioClip = QSharedPointer<Audio::AudioClip>::create("", mTrackToTest.get());
    mTrackToTest->addClip(audioClip);

    mTrackToTest->removeClip(audioClip);
    EXPECT_EQ(mTrackToTest->getClips().size(), 0);
}
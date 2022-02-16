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

TEST_F(TrackTests, IsLooping)
{
    EXPECT_FALSE(mTrackToTest->isLooping());
    mTrackToTest->setLooping(true);
    EXPECT_FALSE(mTrackToTest->isLooping());
}

TEST_F(TrackTests, PrepareToPlay)
{
    mTrackToTest->prepareToPlay(1024, 48000);
    EXPECT_EQ(mTrackToTest->getNextReadPosition(), 0);
}

TEST_F(TrackTests, ApplyGainToBuffer)
{
    mTrackToTest->getTrackProperties()->setVolume(2);
    auto *buffer = new juce::AudioSampleBuffer(2, 1);
    buffer->getArrayOfWritePointers()[0][0] = 0.2;
    buffer->getArrayOfWritePointers()[1][0] = 0.2;

    mTrackToTest->applyGainToBuffer(buffer);
    EXPECT_EQ(buffer->getArrayOfReadPointers()[0][0], 0.4f);
    EXPECT_EQ(buffer->getArrayOfReadPointers()[1][0], 0.4f);

    delete buffer;
}

TEST_F(TrackTests, ApplyPanToBuffer)
{
    mTrackToTest->getTrackProperties()->setPan(-1);
    auto *buffer = new juce::AudioSampleBuffer(2, 1);
    buffer->getArrayOfWritePointers()[0][0] = 0.2;
    buffer->getArrayOfWritePointers()[1][0] = 0.2;

    mTrackToTest->applyPanToBuffer(buffer);
    EXPECT_EQ(buffer->getArrayOfReadPointers()[0][0], 0.4f);
    EXPECT_EQ(buffer->getArrayOfReadPointers()[1][0], 0.f);

    delete buffer;
}

TEST_F(TrackTests, GetNextReadPosition)
{
    mProjectToTest->createAudioClip(mTrackToTest, "");

    auto *buffer = new juce::AudioSampleBuffer(2, 1024);
    juce::AudioSourceChannelInfo info(buffer, 0, 1024);

    mTrackToTest->setNextReadPosition(0);
    EXPECT_EQ(mTrackToTest->getNextReadPosition(), 0);
    EXPECT_EQ(mTrackToTest->getClips().at(0)->getReadPosition(), 0);

    mTrackToTest->getNextAudioBlock(info);
    EXPECT_EQ(mTrackToTest->getNextReadPosition(), 1024);
    EXPECT_EQ(mTrackToTest->getClips().at(0)->getReadPosition(), 1024);

    delete buffer;
}

TEST_F(TrackTests, GetTotalLength)
{
    EXPECT_EQ(mTrackToTest->getTotalLength(), 0);

    int clipID = mProjectToTest->createMIDIClip(mTrackToTest);
    auto clip = mProjectToTest->getClips().at(clipID);
    clip->getClipProperties()->setLengthInSamples(1000);
    clip->getClipProperties()->setPositionInSamples(2000);

    EXPECT_EQ(mTrackToTest->getTotalLength(), 1000 + 2000);
}
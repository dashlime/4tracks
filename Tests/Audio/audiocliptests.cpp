#include "audioenginetests.h"

TEST_F(AudioClipTests, GetType)
{
    EXPECT_EQ(mAudioClip->getType(), Audio::Clip::AUDIO_CLIP);
}

TEST_F(AudioClipTests, GetAudioBuffer)
{
    // get audio buffer without loaded data
    EXPECT_TRUE(mAudioClip->getAudioBuffer().isNull());
}

TEST_F(AudioClipTests, UpdateNextReadPosition)
{
    auto *buffer = new juce::AudioSampleBuffer(2, 1024);
    juce::AudioSourceChannelInfo info(buffer, 0, 1024);

    mAudioClip->setReadPosition(0);
    EXPECT_EQ(mAudioClip->getReadPosition(), 0);

    mAudioClip->getNextAudioBlock(info);
    EXPECT_EQ(mAudioClip->getReadPosition(), 1024);

    delete buffer;
}
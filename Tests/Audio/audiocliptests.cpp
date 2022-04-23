#include "audioenginetests.h"

TEST_F(AudioClipTests, GetType)
{
    EXPECT_EQ(mAudioClip->getType(), Audio::Clip::AUDIO_CLIP);
}

TEST_F(AudioClipTests, GetAudioBuffer)
{
    // get audio buffer without loaded data
    EXPECT_TRUE(mAudioClip->getAudioResource()->getAudioData().isNull());
}

TEST_F(AudioClipTests, GetNextAudioBlock)
{
    auto clipBuffer = QSharedPointer<juce::AudioSampleBuffer>::create(1, 1);

    mAudioClip.reset(new Audio::AudioClip(QSharedPointer<Audio::AudioResource>::create(clipBuffer, ""), mParentTrack.get()));
    mAudioClip->prepareToPlay(1024, 44100);

    auto *buffer = new juce::AudioSampleBuffer(2, 1024);
    juce::AudioSourceChannelInfo info(buffer, 0, 1024);

    mAudioClip->getClipProperties()->setLengthInSamples(1);
    mAudioClip->getClipProperties()->setEndOffset(1);

    mAudioClip->setReadPosition(0);

    mAudioClip->setReadPosition(0);
    EXPECT_EQ(mAudioClip->getReadPosition(), 0);

    mAudioClip->getNextAudioBlock(info);
    EXPECT_EQ(mAudioClip->getReadPosition(), 1024);

    delete buffer;

    mAudioClip->releaseResources();
}
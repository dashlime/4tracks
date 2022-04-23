#include "audioenginetests.h"

TEST_F(MixerAudioSourceTests, IsLooping)
{
    EXPECT_FALSE(mMixerAudioSource->isLooping());
}

TEST_F(MixerAudioSourceTests, GetNextAudioBlock)
{
    int trackID = mProject->createTrack("Test track");
    auto track1 = mProject->getTrackByIndex(trackID);

    trackID = mProject->createTrack("Test track");
    auto track2 = mProject->getTrackByIndex(trackID);

    auto buffer = QSharedPointer<juce::AudioSampleBuffer>::create(1, 1);
    juce::AudioSourceChannelInfo info(buffer.get(), 0, 1);

    mMixerAudioSource->getNextAudioBlock(info);
    EXPECT_EQ(buffer->getSample(0, 0), 0);

    auto buffer1 = QSharedPointer<juce::AudioSampleBuffer>::create(1, 1);
    buffer1->setSample(0, 0, 0.5f);

    auto resource1 = QSharedPointer<Audio::AudioResource>::create(buffer1, "");
    auto clip1 = QSharedPointer<Audio::AudioClip>::create(resource1, track1.get());
    track1->addClip(clip1);
    mMixerAudioSource->addInputSource(track1.get());

    track1->setNextReadPosition(0);
    mMixerAudioSource->getNextAudioBlock(info);
    EXPECT_EQ(buffer->getSample(0, 0), 0.5);

    auto buffer2 = QSharedPointer<juce::AudioSampleBuffer>::create(1, 1);
    buffer2->setSample(0, 0, 1.f);

    auto resource2 = QSharedPointer<Audio::AudioResource>::create(buffer2, "");
    auto clip2 = QSharedPointer<Audio::AudioClip>::create(resource2, track2.get());
    track2->addClip(clip2);
    mMixerAudioSource->addInputSource(track2.get());

    track1->setNextReadPosition(0);
    track2->setNextReadPosition(0);
    mMixerAudioSource->getNextAudioBlock(info);
    EXPECT_EQ(buffer->getSample(0, 0), 1.5);

    mMixerAudioSource->releaseResources();
}
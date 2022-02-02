#include "audioclip.h"

namespace Audio
{

AudioClip::AudioClip(const QString &filePath, Track *parentTrack)
    : Clip(filePath, filePath, parentTrack)
{
    AudioFileLoader loader;
    mAudioBuffer = loader.loadFile(QFile(filePath));

    if (mAudioBuffer) {
        mClipProperties->setLengthInSamples(mAudioBuffer->getNumSamples());

        mAudioSource.reset(new juce::MemoryAudioSource(*mAudioBuffer, false));
    }
}

Clip::Type AudioClip::getType() const
{
    return Clip::AUDIO_CLIP;
}

QSharedPointer<juce::AudioBuffer<float>> AudioClip::getAudioBuffer()
{
    return mAudioBuffer;
}

void AudioClip::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    if (mAudioSource)
        mAudioSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioClip::releaseResources()
{
    if (mAudioSource)
        mAudioSource->releaseResources();
}

void AudioClip::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    if (mAudioSource)
        mAudioSource->getNextAudioBlock(bufferToFill);

    mNextReadPosition += bufferToFill.numSamples;
}

void AudioClip::nextReadPositionChanged()
{
    if (mAudioSource)
        mAudioSource->setNextReadPosition(mNextReadPosition - mClipProperties->getPositionInSamples());
}

}
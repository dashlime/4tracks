#include "audioclip.h"

namespace Audio
{

AudioClip::AudioClip(const QString &filePath, Track *parentTrack)
    : Clip(filePath, parentTrack)
{
    AudioFileLoader loader;
    auto buffer = loader.loadFile(QFile(filePath));

    mAudioResource = QSharedPointer<AudioResource>::create(buffer, filePath);

    if (buffer) {
        mClipProperties->setLengthInSamples(buffer->getNumSamples());
        mClipProperties->setEndOffset(buffer->getNumSamples());

        mAudioSource.reset(new juce::MemoryAudioSource(*buffer, false));
    }
}

AudioClip::AudioClip(const QSharedPointer<AudioResource> &resource, Track *parentTrack)
    : Clip(resource->getSourceFilePath(), parentTrack), mAudioResource(resource)
{
    mClipProperties->setLengthInSamples(resource->getAudioData()->getNumSamples());
    mClipProperties->setEndOffset(resource->getAudioData()->getNumSamples());

    mAudioSource.reset(new juce::MemoryAudioSource(*resource->getAudioData(), false));
}

QSharedPointer<AudioResource> AudioClip::getAudioResource() const
{
    return mAudioResource;
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
    bufferToFill.clearActiveBufferRegion();

    if (mAudioSource)
        if (mNextReadPosition > mClipProperties->getPositionInSamples() + mClipProperties->getStartOffset()
            && mNextReadPosition < mClipProperties->getPositionInSamples() + mClipProperties->getEndOffset())
            mAudioSource->getNextAudioBlock(bufferToFill);


    mNextReadPosition += bufferToFill.numSamples;
}

Clip::Type AudioClip::getType() const
{
    return Clip::AUDIO_CLIP;
}

void AudioClip::nextReadPositionChanged()
{
    if (mAudioSource)
        mAudioSource->setNextReadPosition(mNextReadPosition - mClipProperties->getPositionInSamples());
}

}

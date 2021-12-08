#include "audioclip.h"

namespace Audio {

AudioClip::AudioClip(std::shared_ptr<AudioTrack> parentTrack, QString filePath) : Clip(parentTrack)
{
    mSourceFilePath = filePath;
    mName = filePath;

    load();
}

int AudioClip::getType() const
{
    return Clip::AUDIO_CLIP;
}

void AudioClip::load()
{
    juce::AudioFormatManager formatManager;
    formatManager.registerFormat(new juce::WavAudioFormat(), true);

    juce::File file = juce::File(mSourceFilePath.toStdString());
    juce::AudioFormatReader *reader = formatManager.createReaderFor(file);

    mAudioBuffer.setSize(reader->numChannels, reader->lengthInSamples);

    reader->read(&mAudioBuffer, 0, reader->lengthInSamples, 0, true, true);
    mAudioSource = std::make_unique<juce::MemoryAudioSource>(mAudioBuffer, false);

    mResamplingAudioSource = std::make_unique<ResamplingPositionableAudioSource>(mAudioSource.get(), false, reader->sampleRate, DEFAULT_SAMPLE_RATE, reader->numChannels);

    mLengthInSamples = mResamplingAudioSource->getTotalLength();

    delete reader;
}

juce::AudioBuffer<float>& AudioClip::getAudioBuffer()
{
    return mAudioBuffer;
}

void AudioClip::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mResamplingAudioSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioClip::releaseResources()
{
    mResamplingAudioSource->releaseResources();
}

void AudioClip::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    mResamplingAudioSource->getNextAudioBlock(bufferToFill);
    mNextReadPosition = mResamplingAudioSource->getNextReadPosition() + getPositionInSamples();
}

void AudioClip::nextReadPositionChanged()
{
    mResamplingAudioSource->setNextReadPosition(mNextReadPosition - getPositionInSamples());
}

}

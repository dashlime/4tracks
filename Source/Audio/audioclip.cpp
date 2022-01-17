#include "audioclip.h"

namespace Audio
{

AudioClip::AudioClip(const std::weak_ptr<Track>& parentTrack, const QString& filePath)
    : Clip(parentTrack)
{
    mSourceFilePath = filePath;
    mName = filePath;

    load();
}

QString AudioClip::getPath() const
{
    return mSourceFilePath;
}

int AudioClip::getType() const
{
    return Clip::AUDIO_CLIP;
}

void AudioClip::load()
{
    if (mSourceFilePath == "")
        return;

    juce::AudioFormatManager formatManager;
    formatManager.registerFormat(new juce::WavAudioFormat(), true);

    juce::File file = juce::File(mSourceFilePath.toStdString());
    juce::AudioFormatReader *reader = formatManager.createReaderFor(file);

    juce::AudioSampleBuffer temp;

    mAudioBuffer.clear();
    temp.clear();

    double ratio = reader->sampleRate / DEFAULT_SAMPLE_RATE;

    temp.setSize((int) reader->numChannels, (int) reader->lengthInSamples);
    mAudioBuffer.setSize((int) reader->numChannels, (int) (((double) reader->lengthInSamples) / ratio));

    reader->read(&temp, 0, (int) reader->lengthInSamples, 0, true, true);

    std::unique_ptr<juce::LagrangeInterpolator> resampler = std::make_unique<juce::LagrangeInterpolator>();

    const float **inputs = temp.getArrayOfReadPointers();
    float **outputs = mAudioBuffer.getArrayOfWritePointers();
    for (int c = 0; c < mAudioBuffer.getNumChannels(); c++) {
        resampler->reset();
        resampler->process(ratio, inputs[c], outputs[c], mAudioBuffer.getNumSamples());
    }

    mLengthInSamples = mAudioBuffer.getNumSamples();
    mAudioSource = std::make_unique<juce::MemoryAudioSource>(mAudioBuffer, false);

    delete reader;
}

juce::AudioBuffer<float> &AudioClip::getAudioBuffer()
{
    return mAudioBuffer;
}

void AudioClip::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    mAudioSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioClip::releaseResources()
{
    mAudioSource->releaseResources();
}

void AudioClip::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    mAudioSource->getNextAudioBlock(bufferToFill);
    mNextReadPosition = mAudioSource->getNextReadPosition() + getPositionInSamples();
}

void AudioClip::nextReadPositionChanged()
{
    mAudioSource->setNextReadPosition(mNextReadPosition - getPositionInSamples());
}

}

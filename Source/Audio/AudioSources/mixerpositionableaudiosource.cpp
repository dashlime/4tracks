#include "mixerpositionableaudiosource.h"

MixerPositionableAudioSource::MixerPositionableAudioSource(double sampleRate)
    : currentSampleRate(sampleRate), bufferSizeExpected(0)
{}

MixerPositionableAudioSource::~MixerPositionableAudioSource()
{
    removeAllInputs();
}

// ------------------------------ From MixerAudioSource class ------------------------------
void MixerPositionableAudioSource::addInputSource(juce::PositionableAudioSource *input, const bool deleteWhenRemoved)
{
    if (input != nullptr && !inputs.contains(input)) {
        double localRate;
        int localBufferSize;

        {
            const juce::ScopedLock sl(lock);
            localRate = currentSampleRate;
            localBufferSize = bufferSizeExpected;
        }

        if (localRate > 0.0)
            input->prepareToPlay(localBufferSize, localRate);

        const juce::ScopedLock sl(lock);

        inputsToDelete.setBit(inputs.size(), deleteWhenRemoved);
        inputs.add(input);
    }
}

void MixerPositionableAudioSource::removeInputSource(juce::PositionableAudioSource *const input)
{
    if (input != nullptr) {
        std::unique_ptr<juce::PositionableAudioSource> toDelete;

        {
            const juce::ScopedLock sl(lock);
            const int index = inputs.indexOf(input);

            if (index < 0)
                return;

            if (inputsToDelete[index])
                toDelete.reset(input);

            inputsToDelete.shiftBits(-1, index);
            inputs.remove(index);
        }

        input->releaseResources();
    }
}

void MixerPositionableAudioSource::removeAllInputs()
{
    juce::OwnedArray<juce::PositionableAudioSource> toDelete;

    {
        const juce::ScopedLock sl(lock);

        for (int i = inputs.size(); --i >= 0;)
            if (inputsToDelete[i])
                toDelete.add(inputs.getUnchecked(i));

        inputs.clear();
    }

    for (int i = toDelete.size(); --i >= 0;)
        toDelete.getUnchecked(i)->releaseResources();
}

void MixerPositionableAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    tempBuffer.setSize(2, samplesPerBlockExpected);

    const juce::ScopedLock sl(lock);

    currentSampleRate = sampleRate;
    bufferSizeExpected = samplesPerBlockExpected;

    for (int i = inputs.size(); --i >= 0;)
        inputs.getUnchecked(i)->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MixerPositionableAudioSource::releaseResources()
{
    const juce::ScopedLock sl(lock);

    for (int i = inputs.size(); --i >= 0;)
        inputs.getUnchecked(i)->releaseResources();

    tempBuffer.setSize(2, 0);

    currentSampleRate = 0;
    bufferSizeExpected = 0;
}

void MixerPositionableAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &info)
{
    const juce::ScopedLock sl(lock);

    if (inputs.size() > 0) {
        inputs.getUnchecked(0)->getNextAudioBlock(info);

        if (inputs.size() > 1) {
            tempBuffer.setSize(juce::jmax(1, info.buffer->getNumChannels()),
                               info.buffer->getNumSamples());

            juce::AudioSourceChannelInfo info2(&tempBuffer, 0, info.numSamples);

            for (int i = 1; i < inputs.size(); ++i) {
                inputs.getUnchecked(i)->getNextAudioBlock(info2);

                for (int chan = 0; chan < info.buffer->getNumChannels(); ++chan)
                    info.buffer->addFrom(chan, info.startSample, tempBuffer, chan, 0, info.numSamples);
            }
        }
    }
    else {
        info.clearActiveBufferRegion();
    }
}

// END ------------------------------ From MixerAudioSource class ------------------------------ END

juce::int64 MixerPositionableAudioSource::getTotalLength() const
{
    juce::int64 maxLength = 0;
    for (juce::PositionableAudioSource *input: inputs) {
        juce::int64 length = input->getTotalLength();
        if (length > maxLength) {
            maxLength = length;
        }
    }

    return maxLength;
}

juce::int64 MixerPositionableAudioSource::getNextReadPosition() const
{
    juce::int64 maxPosition = 0;
    for (juce::PositionableAudioSource *input: inputs) {
        juce::int64 pos = input->getNextReadPosition();

        if (pos > maxPosition) {
            maxPosition = pos;
        }
    }

    return maxPosition;
}

void MixerPositionableAudioSource::setNextReadPosition(juce::int64 newPosition)
{
    for (juce::PositionableAudioSource *input: inputs) {
        input->setNextReadPosition(newPosition);
    }
}

bool MixerPositionableAudioSource::isLooping() const
{
    return false;
}

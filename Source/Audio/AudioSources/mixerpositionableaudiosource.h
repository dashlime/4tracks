#ifndef MIXERPOSITIONABLEAUDIOSOURCE
#define MIXERPOSITIONABLEAUDIOSOURCE

#pragma once

#include "Audio/JuceIncludes.h"
#include <iostream>

namespace Audio
{

class MixerPositionableAudioSource: public juce::PositionableAudioSource
{
public:
    explicit MixerPositionableAudioSource(double sampleRate);
    ~MixerPositionableAudioSource() override;

    //Base functions
    void addInputSource(juce::PositionableAudioSource *input);
    void removeInputSource(juce::PositionableAudioSource *input);
    void removeAllInputs();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &info) override;

    //New functions
    juce::int64 getTotalLength() const override;
    juce::int64 getNextReadPosition() const override;
    void setNextReadPosition(juce::int64 newPosition) override;
    bool isLooping() const override;

private:
    juce::Array<juce::PositionableAudioSource *> inputs;
    juce::CriticalSection lock;
    juce::AudioBuffer<float> tempBuffer;
    double currentSampleRate;
    int bufferSizeExpected;
};

}

#endif

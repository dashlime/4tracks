#ifndef DEF_MIXERPOSITIONABLEAUDIOSOURCE
#define DEF_MIXERPOSITIONABLEAUDIOSOURCE

#pragma once

#include "Audio/JuceIncludes.h"
#include <iostream>

class MixerPositionableAudioSource: public juce::PositionableAudioSource
{
public:
    explicit MixerPositionableAudioSource(double sampleRate);
    ~MixerPositionableAudioSource() override;

    //Base functions
    void addInputSource(juce::PositionableAudioSource *input, bool deleteWhenRemoved);
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
    juce::BigInteger inputsToDelete;
    juce::CriticalSection lock;
    juce::AudioBuffer<float> tempBuffer;
    double currentSampleRate;
    int bufferSizeExpected;
};

#endif

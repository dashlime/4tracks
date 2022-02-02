#ifndef DEF_MIXERPOSITIONABLEAUDIOSOURCE
#define DEF_MIXERPOSITIONABLEAUDIOSOURCE

#pragma once

#include "Audio/JuceIncludes.h"
#include <iostream>

class MixerPositionableAudioSource: public juce::PositionableAudioSource
{
public:
    MixerPositionableAudioSource(double sampleRate);
    ~MixerPositionableAudioSource();

    //Base functions
    void addInputSource(juce::PositionableAudioSource *input, const bool deleteWhenRemoved);
    void removeInputSource(juce::PositionableAudioSource *const input);
    void removeInputSource(int const input);
    void removeAllInputs();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void releaseResources();
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &info);

    //New functions
    juce::int64 getTotalLength() const;
    juce::int64 getNextReadPosition() const;
    void setNextReadPosition(juce::int64 newPosition);
    bool isLooping() const;

private:
    juce::Array<juce::PositionableAudioSource *> inputs;
    juce::BigInteger inputsToDelete;
    juce::CriticalSection lock;
    juce::AudioBuffer<float> tempBuffer;
    double currentSampleRate;
    int bufferSizeExpected;
};

#endif

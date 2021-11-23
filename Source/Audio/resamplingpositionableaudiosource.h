#pragma once

#include "JuceIncludes.h"

class ResamplingPositionableAudioSource : public juce::PositionableAudioSource
{
public:
    ResamplingPositionableAudioSource(juce::PositionableAudioSource *inputSource, bool deleteInputWhenDeleted, double sourceSampleRate, double sampleRate = 44100, int numChannels = 2);
    ~ResamplingPositionableAudioSource() override;

    void setResamplingRatio(double samplesInPerOutputSample);
    double getResamplingRatio() const noexcept                  { return ratio; }

    void flushBuffers();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo&) override;

    void setNextReadPosition(juce::int64 newPosition) override
    {
        input->setNextReadPosition((juce::int64) ((double) newPosition * sourceSampleRate / sampleRate));
        flushBuffers();
    }

    juce::int64 getNextReadPosition() const override
    {
        const double r = (sampleRate > 0 && sourceSampleRate > 0) ? sampleRate / sourceSampleRate : 1.0;
        return (juce::int64) ((double) input->getNextReadPosition() * r);
    }

    juce::int64 getTotalLength() const override
    {
        const double r = (sampleRate > 0 && sourceSampleRate > 0) ? sampleRate / sourceSampleRate : 1.0;
        return (juce::int64) ((double) input->getTotalLength() * r);
    }

    bool isLooping() const override { return input->isLooping(); }
    void setLooping(bool shouldLoop) override { input->setLooping(shouldLoop); }

private:
    juce::OptionalScopedPointer<juce::PositionableAudioSource> input;
    double ratio = 1.0, lastRatio = 1.0;
    juce::AudioBuffer<float> buffer;
    int bufferPos = 0, sampsInBuffer = 0;
    double subSampleOffset = 0.0;
    double coefficients[6];
    juce::SpinLock ratioLock;
    juce::CriticalSection callbackLock;
    const int numChannels;
    juce::HeapBlock<float*> destBuffers;
    juce::HeapBlock<const float*> srcBuffers;

    double sourceSampleRate = 0.0, sampleRate = 0.0;

    void setFilterCoefficients (double c1, double c2, double c3, double c4, double c5, double c6);
    void createLowPass (double proportionalRate);

    struct FilterState
    {
        double x1, x2, y1, y2;
    };

    juce::HeapBlock<FilterState> filterStates;
    void resetFilters();

    void applyFilter (float* samples, int num, FilterState& fs);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResamplingPositionableAudioSource)
};

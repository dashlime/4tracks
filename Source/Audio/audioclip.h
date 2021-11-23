#ifndef AUDIOCLIP_H
#define AUDIOCLIP_H

#include "clip.h"
#include "resamplingpositionableaudiosource.h"
#include "Constants.h"

namespace Audio {

class AudioClip : public Clip, public juce::PositionableAudioSource
{
public:
    AudioClip(std::shared_ptr<AudioTrack> parentTrack, QString filePath);

    int getType() const override;
    juce::AudioBuffer<float>& getAudioBuffer();

    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
private:
    void load();

    juce::AudioBuffer<float> mAudioBuffer;
    std::unique_ptr<juce::MemoryAudioSource> mAudioSource;
    std::unique_ptr<ResamplingPositionableAudioSource> mResamplingAudioSource;
};

}

#endif // AUDIOCLIP_H

#ifndef AUDIOCLIP_H
#define AUDIOCLIP_H

#include "clip.h"
#include "resamplingpositionableaudiosource.h"
#include "Constants.h"

namespace Audio
{

class AudioClip: public Clip, public juce::AudioSource
{
public:
    AudioClip(std::weak_ptr<Track> parentTrack, QString filePath);

    QString getPath() const;

    int getType() const override;

    juce::AudioBuffer<float> &getAudioBuffer();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    void nextReadPositionChanged() override;
private:
    void load();

    juce::AudioBuffer<float> mAudioBuffer;
    std::unique_ptr<juce::MemoryAudioSource> mAudioSource;
};

}

#endif // AUDIOCLIP_H

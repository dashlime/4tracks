#ifndef AUDIOCLIP_H
#define AUDIOCLIP_H

#include "clip.h"
#include "Audio/AudioSources/resamplingpositionableaudiosource.h"
#include "Audio/Utils/audiofileloader.h"
#include "Constants.h"
#include "clipproperties.h"

namespace Audio
{

class AudioClip: public Clip, public juce::AudioSource
{
public:
    AudioClip(const QString &filePath, Track *parentTrack);

    [[nodiscard]] Type getType() const override;

    QSharedPointer<juce::AudioBuffer<float>> getAudioBuffer();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    void nextReadPositionChanged() override;
private:
    QSharedPointer<juce::AudioBuffer<float>> mAudioBuffer;
    QScopedPointer<juce::MemoryAudioSource> mAudioSource;
};

}

#endif // AUDIOCLIP_H

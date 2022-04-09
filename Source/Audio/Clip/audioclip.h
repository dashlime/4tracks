#ifndef AUDIOCLIP_H
#define AUDIOCLIP_H

#include "clip.h"
#include "Audio/AudioSources/resamplingpositionableaudiosource.h"
#include "Audio/Resources/audiofileloader.h"
#include "Constants.h"
#include "clipproperties.h"
#include "Audio/Resources/audioresource.h"

namespace Audio
{

class AudioClip: public Clip, public juce::AudioSource
{
public:
    AudioClip(const QString &filePath, Track *parentTrack);
    AudioClip(const QSharedPointer<AudioResource> &resource, Track *parentTrack);

    [[nodiscard]] QSharedPointer<AudioResource> getAudioResource() const;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    [[nodiscard]] Type getType() const override;
    void nextReadPositionChanged() override;
    void removeArea(int startNote, int nbNotes, juce::int64 relativeStartSample, juce::int64 nbSamples) override;
private:
    QSharedPointer<AudioResource> mAudioResource;
    QScopedPointer<juce::MemoryAudioSource> mAudioSource;
};

}

#endif // AUDIOCLIP_H

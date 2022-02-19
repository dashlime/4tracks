#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include "Audio/JuceIncludes.h"
#include "Constants.h"
#include "Audio/Clip/clip.h"
#include "Audio/Clip/audioclip.h"
#include "Audio/AudioSources/mixerpositionableaudiosource.h"
#include "trackproperties.h"

#include <QString>
#include <QDebug>

namespace Audio
{

class Project;

class Track: public QObject, public juce::PositionableAudioSource
{
Q_OBJECT

public:
    Track(const QString &name, int index, Project *parentProject);

    [[nodiscard]] QSharedPointer<TrackProperties> getTrackProperties() const;

    [[nodiscard]] int getType() const;

    bool addClip(const QSharedPointer<Clip> &clip);
    QVector<QSharedPointer<Clip>> getClips();

    void removeClip(const QSharedPointer<Clip> &clipToRemove);

    void removeArea(int startSample, int nbSamples);

    void applyGainToBuffer(juce::AudioSampleBuffer *buffer);
    void applyPanToBuffer(juce::AudioSampleBuffer *buffer);

    void setNextReadPosition(juce::int64 newPosition) override;
    [[nodiscard]] juce::int64 getNextReadPosition() const override;
    [[nodiscard]] juce::int64 getTotalLength() const override;
    [[nodiscard]] bool isLooping() const override;
    void setLooping(bool) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    enum Type
    {
        ANY_TRACK, AUDIO_TRACK, MIDI_TRACK
    };

signals:
    void clipRemoved(const QSharedPointer<Clip> &clipRemoved);

private:
    void resizeClipsWhenClipAdded(int newClipIndex);

    QSharedPointer<TrackProperties> mProperties;

    QVector<QSharedPointer<Clip>> mClips;
    QSharedPointer<Clip> mClipPlaying;

    juce::int64 mPositionInSamples = 0;
};

}

#endif // AUDIOTRACK_H

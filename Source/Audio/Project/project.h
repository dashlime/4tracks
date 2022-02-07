#ifndef TIMELINE_H
#define TIMELINE_H

#include "Audio/AudioSources/mixerpositionableaudiosource.h"
#include "Audio/Track/track.h"
#include "Audio/JuceIncludes.h"
#include "Constants.h"
#include "projectproperties.h"
#include "Audio/Clip/midiclip.h"

#include <QVector>

namespace Audio
{

class Project: public QObject, public juce::PositionableAudioSource
{
Q_OBJECT

public:
    explicit Project(const QString &projectName);

    QSharedPointer<ProjectProperties> getProjectProperties() const;

    void addTrack(const QSharedPointer<Track> &newTrack);
    QVector<QSharedPointer<Track>> getTracks() const;

    bool addClip(const QSharedPointer<Clip> &newClip);
    QVector<QSharedPointer<Clip>> getClips() const;

    int createTrack(const QString &trackName);
    int createAudioClip(const QSharedPointer<Track>& parentTrack, const QString &filePath);
    int createMIDIClip(const QSharedPointer<Track>& parentTrack);

    QSharedPointer<Track> getTrackByIndex(int trackIndex) const;

    [[nodiscard]] bool canRemoveTrack() const;
    void removeTrack(QSharedPointer<Track> trackToRemove);

    void removeClip(QSharedPointer<Clip> clipToRemove);

    void clearAllTracks();

    void rearrangeTrackIndexes();

    void play();
    void pause();
    void stop();

    bool isPlaying() const;

    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

signals:
    void trackAdded(int index);
    void trackRemoved(int index);
    void clipAdded(int clipID);
    void clipRemoved(int clipID);

private:
    QVector<QSharedPointer<Track>> mTracks;
    QVector<QSharedPointer<Clip>> mClips;
    MixerPositionableAudioSource mMixerAudioSource;
    QSharedPointer<ProjectProperties> mProjectProperties;

    bool mPlaying = false;
};

}

#endif // TIMELINE_H

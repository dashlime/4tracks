#ifndef TIMELINE_H
#define TIMELINE_H

#include "Audio/AudioSources/mixerpositionableaudiosource.h"
#include "Audio/Track/track.h"
#include "Audio/JuceIncludes.h"
#include "Constants.h"
#include "projectproperties.h"

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

    int createTrack(const QString &trackName);

    QSharedPointer<Track> getTrackByIndex(int trackIndex) const;

    [[nodiscard]] bool canRemoveTrack() const;
    void removeTrack(QSharedPointer<Track> trackToRemove);

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

private:
    QVector<QSharedPointer<Track>> mTracks;
    MixerPositionableAudioSource mMixerAudioSource;
    QSharedPointer<ProjectProperties> mProjectProperties;

    bool mPlaying = false;
};

}

#endif // TIMELINE_H

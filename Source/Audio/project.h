#ifndef TIMELINE_H
#define TIMELINE_H

#include "mixerpositionableaudiosource.h"
#include "track.h"
#include "JuceIncludes.h"
#include "Constants.h"


#include <QVector>

namespace Audio
{

class Project: public juce::PositionableAudioSource
{
public:
    explicit Project(const QString &projectName);

    QString getProjectName() const;
    void setProjectName(const QString &newProjectName);

    void addTrack(const std::shared_ptr<Track> &newTrack);
    std::vector<std::shared_ptr<Track>> getTracks() const;

    std::shared_ptr<Track> getTrackByIndex(int trackIndex) const;

    [[nodiscard]] bool canRemoveTrack() const;
    void removeTrack(std::shared_ptr<Track> &trackToRemove);

    void clearAllTracks();

    void rearrangeTrackIndexes();

    void play();
    void pause();
    void stop();

    bool isPlaying() const;

    void updateSavedState(int newSavedState);
    int getSavedState() const;

    void setBpm(double newBpm, bool propagateEvent = true);
    double getBpm() const;

    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    enum SavedState
    {
        SAVED, UNSAVED
    };

    std::function<void()> savedStateChanged;
    std::function<void()> projectNameChanged;
    std::function<void()> trackAdded;
    std::function<void()> trackRemoved;
    std::function<void()> bpmChanged;
private:
    std::vector<std::shared_ptr<Track>> mTracks;
    MixerPositionableAudioSource mMixerAudioSource;

    bool mPlaying = false;
    double mBpm = 120;

    int mSavedState = SAVED;

    QString mProjectName;
};

}

#endif // TIMELINE_H

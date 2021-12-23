#ifndef TIMELINE_H
#define TIMELINE_H

#include "mixerpositionableaudiosource.h"
#include "audiotrack.h"
#include "JuceIncludes.h"
#include "Constants.h"


#include <QVector>

namespace Audio {

class Project : public juce::PositionableAudioSource
{
public:
    Project(QString projectName);

    QString getProjectName() const;
    void setProjectName(QString newProjectName);

    void addTrack(std::shared_ptr<AudioTrack> newTrack);
    void play();
    void pause();
    void stop();

    void updateSavedState(int newSavedState);
    int getSavedState() const;

    void setBpm(double newBpm);
    double getBpm() const;

    QVector<std::shared_ptr<AudioTrack>> getTracks();

    bool isPlaying() const;

    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    enum SavedState { SAVED, UNSAVED };

    std::function<void()> savedStateChanged;
private:
    QVector<std::shared_ptr<AudioTrack>> mTracks;
    MixerPositionableAudioSource mMixerAudioSource;

    bool mPlaying = false;
    double mBpm = 120;

    int mSavedState = SAVED;

    QString mProjectName;
};

}

#endif // TIMELINE_H

#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include "JuceIncludes.h"
#include "Constants.h"
#include "clip.h"
#include "audioclip.h"
#include "mixerpositionableaudiosource.h"

#include <QString>
#include <QDebug>

namespace Audio
{

class Track: public juce::PositionableAudioSource
{
public:
    Track(QString name, int index);

    QString getName() const;
    int getType() const;

    bool addClip(std::shared_ptr<Clip> clip);
    std::vector<std::shared_ptr<Clip>> getClips();

    int getIndex() const;
    void setIndex(int newPosition);

    void setVolume(double newVolume);
    double getVolume() const;

    void setPan(double newPan);
    double getPan() const;

    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    std::function<void()> clipAdded;
    std::function<void()> clipRemoved;
    enum Type
    {
        ANY_TRACK, AUDIO_TRACK, MIDI_TRACK
    };

private:
    void resizeClipsWhenClipAdded(int newClipIndex);

    QString mName;
    std::vector<std::shared_ptr<Clip>> mClips;

    std::shared_ptr<Clip> mClipPlaying;

    int mPositionInSamples = 0;
    double mVolume = 1;
    double mPan = 0;
    int mIndex;
};

}

#endif // AUDIOTRACK_H

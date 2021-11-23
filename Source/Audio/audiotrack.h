#ifndef AUDIOTRACK_H
#define AUDIOTRACK_H

#include "JuceIncludes.h"
#include "Constants.h"
#include "clip.h"
#include "audioclip.h"
#include "mixerpositionableaudiosource.h"

#include <QString>
#include <QVector>

namespace Audio {

class AudioTrack : public juce::PositionableAudioSource
{
public:
    AudioTrack(QString name, int index);

    int getType() const;
    bool addClip(std::shared_ptr<Clip> clip);
    QString getName() const;

    int getIndex() const;
    void setIndex(int newPosition);

    QVector<std::shared_ptr<Clip>> getClips();

    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    enum Type { ANY_TRACK, AUDIO_TRACK, MIDI_TRACK };

private:
    void resizeClipsWhenClipAdded(int newClipIndex);

    QString mName;
    QVector<std::shared_ptr<Clip>> mClips;

    std::shared_ptr<Clip> mClipPlaying;

    int mPositionInSamples = 0;
    int mIndex;
};

}

#endif // AUDIOTRACK_H

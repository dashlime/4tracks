#ifndef CLIP_H
#define CLIP_H

#include <QString>

#include "JuceIncludes.h"

namespace Audio
{

class Track;

class Clip
{
public:
    explicit Clip(const std::weak_ptr<Track>& parentTrack);

    void setName(const QString& name);
    [[nodiscard]] QString getName() const;

    [[nodiscard]] QString getSourceFilePath() const;

    [[nodiscard]] juce::int64 getPositionInSamples() const;
    void setClipPositionInSamples(juce::int64 newPosition);

    [[nodiscard]] juce::int64 getLengthInSamples() const;

    void setReadPosition(juce::int64 newPosition);
    [[nodiscard]] juce::int64 getReadPosition() const;

    std::weak_ptr<Track> getParentTrack();

    [[nodiscard]] virtual int getType() const = 0;
    virtual void nextReadPositionChanged()
    {};

    std::function<void()> clipMoved;

    enum Type
    {
        AUDIO_CLIP, MIDI_CLIP
    };
protected:
    QString mName;
    QString mSourceFilePath;
    juce::int64 mPositionInSamples = 0;
    juce::int64 mLengthInSamples = 0;
    juce::int64 mNextReadPosition = 0;

    std::weak_ptr<Track> mParentTrack;
};

}

#endif // CLIP_H

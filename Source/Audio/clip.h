#ifndef CLIP_H
#define CLIP_H

#include <QString>

#include "JuceIncludes.h"

namespace Audio {

class Track;

class Clip
{
public:
    Clip(std::shared_ptr<Audio::Track> parentTrack);

    void setName(QString name);
    QString getName() const;

    QString getSourceFilePath() const;

    int getPositionInSamples() const;
    void setClipPositionInSamples(int newPosition);

    int getLengthInSamples() const;

    void setReadPosition(int newPosition);
    int getReadPosition() const;

    std::shared_ptr<Audio::Track> getParentTrack();

    virtual int getType() const = 0;
    virtual void nextReadPositionChanged() {};

    std::function<void()> onClipMoved;

    enum Type { AUDIO_CLIP, MIDI_CLIP };
protected:
    QString mName;
    QString mSourceFilePath;
    int mPositionInSamples = 0;
    int mLengthInSamples = 0;
    int mNextReadPosition = 0;

    std::shared_ptr<Audio::Track> mParentTrack;
};

}

#endif // CLIP_H

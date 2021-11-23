#ifndef CLIP_H
#define CLIP_H

#include <QString>
#include <QResizeEvent>

#include "JuceIncludes.h"

namespace Audio {

class AudioTrack;

class Clip
{
public:
    Clip(std::shared_ptr<Audio::AudioTrack> parentTrack);

    QString getName() const;
    QString getSourceFilePath() const;
    int getPositionInSamples() const;
    void setClipPositionInSamples(int newPosition);
    int getLengthInSamples() const;

    std::shared_ptr<Audio::AudioTrack> getParentTrack();

    virtual int getType() const = 0;

    enum Type { AUDIO_CLIP, MIDI_CLIP };
protected:
    QString mName;
    QString mSourceFilePath;
    int mPositionInSamples = 0;
    int mLengthInSamples = 0;

    std::shared_ptr<Audio::AudioTrack> mParentTrack;
};

}

#endif // CLIP_H

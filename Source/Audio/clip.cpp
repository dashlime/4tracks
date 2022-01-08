#include "clip.h"

namespace Audio {

Clip::Clip(std::shared_ptr<AudioTrack> parentTrack) : mParentTrack(parentTrack)
{

}

void Clip::setName(QString name)
{
    mName = name;
}

QString Clip::getName() const
{
    return mName;
}

QString Clip::getSourceFilePath() const
{
    return mSourceFilePath;
}

int Clip::getPositionInSamples() const
{
    return mPositionInSamples;
}

void Clip::setClipPositionInSamples(int newPosition)
{
    mPositionInSamples = newPosition;
    onClipMoved();
}

int Clip::getLengthInSamples() const
{
    return mLengthInSamples;
}

void Clip::setReadPosition(int newPosition)
{
    mNextReadPosition = newPosition;
    nextReadPositionChanged();
}

int Clip::getReadPosition() const
{
    return mNextReadPosition;
}

std::shared_ptr<AudioTrack> Clip::getParentTrack()
{
    return mParentTrack;
}

}

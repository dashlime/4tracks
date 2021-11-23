#include "clip.h"

namespace Audio {

Clip::Clip(std::shared_ptr<AudioTrack> parentTrack) : mParentTrack(parentTrack)
{

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
}

int Clip::getLengthInSamples() const
{
    return mLengthInSamples;
}

std::shared_ptr<AudioTrack> Clip::getParentTrack()
{
    return mParentTrack;
}

}

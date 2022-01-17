#include "clip.h"

namespace Audio
{

Clip::Clip(const std::weak_ptr<Track>& parentTrack)
    : mParentTrack(parentTrack)
{}

void Clip::setName(const QString& name)
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

juce::int64 Clip::getPositionInSamples() const
{
    return mPositionInSamples;
}

void Clip::setClipPositionInSamples(juce::int64 newPosition)
{
    mPositionInSamples = newPosition;
    clipMoved();
}

juce::int64 Clip::getLengthInSamples() const
{
    return mLengthInSamples;
}

void Clip::setReadPosition(juce::int64 newPosition)
{
    mNextReadPosition = newPosition;
    nextReadPositionChanged();
}

juce::int64 Clip::getReadPosition() const
{
    return mNextReadPosition;
}

std::weak_ptr<Track> Clip::getParentTrack()
{
    return mParentTrack;
}

}

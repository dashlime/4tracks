#include "clip.h"

namespace Audio
{

Clip::Clip(const QString &name, Track *parentTrack)
    : mClipProperties(QSharedPointer<ClipProperties>::create(parentTrack, name))
{}

void Clip::setReadPosition(juce::int64 newPosition)
{
    mNextReadPosition = newPosition;
    nextReadPositionChanged();
}

QSharedPointer<ClipProperties> Clip::getClipProperties() const
{
    return mClipProperties;
}

juce::int64 Clip::getReadPosition() const
{
    return mNextReadPosition;
}

}

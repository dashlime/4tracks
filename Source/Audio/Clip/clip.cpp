#include "clip.h"

namespace Audio
{

Clip::Clip(const QString &name, const QString &sourceFilePath, Track *parentTrack)
    : mClipProperties(QSharedPointer<ClipProperties>::create(parentTrack, name, sourceFilePath))
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

void Clip::nextReadPositionChanged()
{}

}

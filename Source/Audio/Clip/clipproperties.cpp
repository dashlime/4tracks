#include "clipproperties.h"

namespace Audio
{

ClipProperties::ClipProperties(Track *parentTrack, const QString &name, const QString &sourceFilePath)
    : mParentTrack(parentTrack), mName(name), mSourceFilePath(sourceFilePath)
{}

void ClipProperties::setParentTrack(Track *parentTrack)
{
    mParentTrack = parentTrack;

    emit parentTrackChanged();
    emit savedStateChanged(ProjectProperties::UNSAVED);
}

Track *ClipProperties::getParentTrack() const
{
    return mParentTrack;
}

void ClipProperties::setName(const QString &name)
{
    mName = name;

    emit nameChanged();
    emit savedStateChanged(ProjectProperties::UNSAVED);
}

const QString &ClipProperties::getName() const
{
    return mName;
}

void ClipProperties::setSourceFilePath(const QString &sourceFilePath)
{
    mSourceFilePath = sourceFilePath;

    emit sourceFileChanged();
    emit savedStateChanged(ProjectProperties::UNSAVED);
}

QString ClipProperties::getSourceFilePath() const
{
    return mSourceFilePath;
}

void ClipProperties::setLengthInSamples(juce::int64 lengthInSamples)
{
    mLengthInSamples = lengthInSamples;

    emit lengthChanged();
    emit savedStateChanged(ProjectProperties::UNSAVED);
}

juce::int64 ClipProperties::getLengthInSamples() const
{
    return mLengthInSamples;
}

void ClipProperties::setPositionInSamples(juce::int64 positionInSamples)
{
    mPositionInSamples = positionInSamples;

    emit clipMoved();
    emit savedStateChanged(ProjectProperties::UNSAVED);
}

juce::int64 ClipProperties::getPositionInSamples() const
{
    return mPositionInSamples;
}

}
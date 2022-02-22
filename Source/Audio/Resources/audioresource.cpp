#include "audioresource.h"

namespace Audio
{

AudioResource::AudioResource(const QSharedPointer<juce::AudioSampleBuffer> &data, const QString &sourceFilePath)
    : mAudioData(data), mSourceFilePath(sourceFilePath)
{}

QSharedPointer<juce::AudioSampleBuffer> AudioResource::getAudioData() const
{
    return mAudioData;
}

void AudioResource::setSourceFilePath(const QString &sourceFilePath)
{
    mSourceFilePath = sourceFilePath;
}

QString AudioResource::getSourceFilePath() const
{
    return mSourceFilePath;
}

}
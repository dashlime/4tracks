#ifndef AUDIORESOURCE_H
#define AUDIORESOURCE_H

#include "Audio/JuceIncludes.h"

#include "QSharedPointer"

namespace Audio
{

class AudioResource
{
public:
    AudioResource(const QSharedPointer<juce::AudioSampleBuffer>& data, const QString& sourceFilePath);

    [[nodiscard]] QSharedPointer<juce::AudioSampleBuffer> getAudioData() const;

    void setSourceFilePath(const QString &sourceFilePath);
    [[nodiscard]] QString getSourceFilePath() const;

private:
    QSharedPointer<juce::AudioSampleBuffer> mAudioData;
    QString mSourceFilePath;
};

}

#endif //AUDIORESOURCE_H

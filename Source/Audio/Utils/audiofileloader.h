#ifndef AUDIOFILELOADER_H
#define AUDIOFILELOADER_H

#include <QFile>
#include <QSharedPointer>
#include "Audio/JuceIncludes.h"
#include "Constants.h"

namespace Audio
{

class AudioFileLoader
{
public:
    AudioFileLoader();

    void initialiseAudioFormats();

    [[nodiscard]] QSharedPointer<juce::AudioSampleBuffer> loadFile(const QFile &fileToLoad);

    [[nodiscard]] static bool checkFileName(const QString &fileToCheck);

private:
    juce::AudioFormatManager mFormatManager;
};

}

#endif //AUDIOFILELOADER_H

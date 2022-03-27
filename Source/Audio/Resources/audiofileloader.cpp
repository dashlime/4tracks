#include "audiofileloader.h"

namespace Audio
{

AudioFileLoader::AudioFileLoader()
{
    initialiseAudioFormats();
}

void AudioFileLoader::initialiseAudioFormats()
{
    if (SUPPORTED_AUDIO_FILENAMES.contains(WAV))
        mFormatManager.registerFormat(new juce::WavAudioFormat(), false);

    if (SUPPORTED_AUDIO_FILENAMES.contains(MP3))
        mFormatManager.registerFormat(new juce::MP3AudioFormat(), false);

    if (SUPPORTED_AUDIO_FILENAMES.contains(AIFF))
        mFormatManager.registerFormat(new juce::AiffAudioFormat(), false);
}

QSharedPointer<juce::AudioSampleBuffer> AudioFileLoader::loadFile(const QFile &fileToLoad)
{
    if (!checkFileName(fileToLoad.fileName()))
        return {};

    if (!fileToLoad.exists())
        return {};

    juce::File file = juce::File(fileToLoad.fileName().toStdString());
    juce::AudioFormatReader *reader = mFormatManager.createReaderFor(file);

    auto buffer = QSharedPointer<juce::AudioSampleBuffer>::create();
    buffer->clear();

    juce::AudioSampleBuffer temp;
    temp.clear();

    double ratio = reader->sampleRate / DEFAULT_SAMPLE_RATE;

    temp.setSize((int) reader->numChannels, (int) reader->lengthInSamples);
    buffer->setSize((int) reader->numChannels, int((double) reader->lengthInSamples / ratio));

    reader->read(&temp, 0, (int) reader->lengthInSamples, 0, true, true);

    std::unique_ptr<juce::LagrangeInterpolator> resampler = std::make_unique<juce::LagrangeInterpolator>();

    const float **inputs = temp.getArrayOfReadPointers();
    float **outputs = buffer->getArrayOfWritePointers();
    for (int c = 0; c < buffer->getNumChannels(); c++) {
        resampler->reset();
        resampler->process(ratio, inputs[c], outputs[c], buffer->getNumSamples());
    }

    delete reader;

    return buffer;
}

bool AudioFileLoader::checkFileName(const QString &fileToCheck)
{
    auto stringList = fileToCheck.split(".");
    if (stringList.size() < 2)
        return false;

    if (!SUPPORTED_AUDIO_FILENAMES.contains(stringList[1]))
        return false;

    return true;
}

}
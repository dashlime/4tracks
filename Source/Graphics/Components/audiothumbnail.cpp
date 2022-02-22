#include "audiothumbnail.h"

namespace Graphics
{

AudioThumbnail::AudioThumbnail(const QSharedPointer<Audio::AudioResource> &resource)
    : mAudioResource(resource)
{}

void AudioThumbnail::loadThumbnail(int sourceSamplesPerThumbnailSample)
{
    mThumbnailValues.clear();
    juce::int64 thumbnailSamples = mAudioResource->getAudioData()->getNumSamples() / sourceSamplesPerThumbnailSample;

    for (juce::int64 i = 0; i < thumbnailSamples; i++) {
        float min = 0;
        float max = 0;
        for (int j = 0; j < sourceSamplesPerThumbnailSample; j++) {
            float value =
                mAudioResource->getAudioData()->getSample(0, i * (juce::int64) sourceSamplesPerThumbnailSample + j);
            min = value < min ? value : min;
            max = value > max ? value : max;
        }
        mThumbnailValues.emplace_back(min, max);
    }
}

const std::vector<AudioThumbnail::MinMaxValues> &AudioThumbnail::getThumbnailValues() const
{
    return mThumbnailValues;
}

QSharedPointer<Audio::AudioResource> AudioThumbnail::getAudioResource() const
{
    return mAudioResource;
}

} // namespace Graphics

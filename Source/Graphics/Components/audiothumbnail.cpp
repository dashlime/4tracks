#include "audiothumbnail.h"

namespace Graphics
{

AudioThumbnail::AudioThumbnail() = default;

void AudioThumbnail::loadThumbnail(const QSharedPointer<Audio::AudioClip> &clip, int sourceSamplesPerThumbnailSample)
{
    int thumbnailSamples = clip->getAudioBuffer()->getNumSamples() / sourceSamplesPerThumbnailSample;

    for (int i = 0; i < thumbnailSamples; i++) {
        float min = 0;
        float max = 0;
        for (int j = 0; j < sourceSamplesPerThumbnailSample; j++) {
            float value = clip->getAudioBuffer()->getSample(0, i * sourceSamplesPerThumbnailSample + j);
            min = value < min ? value : min;
            max = value > max ? value : max;
        }
        mThumbnailValues.emplace_back(min, max);
    }
}

void AudioThumbnail::drawThumbnail(QPainter &p, QRect rect)
{
    float verticalCenter = (float) rect.height() / 2;
    float ratio = verticalCenter;

    double thumbnailValuesPerPixel = (double) mThumbnailValues.size() / (double) rect.width();

    p.translate(rect.topLeft());
    for (int i = 0; i < rect.width(); i++) {
        float min = mThumbnailValues.at(i * thumbnailValuesPerPixel).getMinValue();
        float max = mThumbnailValues.at(i * thumbnailValuesPerPixel).getMaxValue();
        p.drawLine(i, int(min * ratio + verticalCenter), i, int(max * ratio + verticalCenter));
    }
}

} // namespace Graphics

#include "audiothumbnail.h"

namespace Graphics
{

AudioThumbnail::AudioThumbnail(const QSharedPointer<Audio::AudioClip> &clip) : mClip(clip)
{
    setupCallbacks();
}

void AudioThumbnail::loadThumbnail(int sourceSamplesPerThumbnailSample)
{
    mThumbnailValues.clear();
    mSourceSamplesPerThumbnailSample = sourceSamplesPerThumbnailSample;

    juce::int64 startOffset = mClip->getClipProperties()->getStartOffset();
    juce::int64 endOffset = mClip->getClipProperties()->getEndOffset();
    juce::int64 thumbnailSamples = (endOffset - startOffset) / sourceSamplesPerThumbnailSample;

    juce::int64 thumbnailOffset = startOffset / sourceSamplesPerThumbnailSample;

    for (juce::int64 i = thumbnailOffset; i < thumbnailSamples + thumbnailOffset; i++) {
        float min = 0;
        float max = 0;
        for (int j = 0; j < sourceSamplesPerThumbnailSample; j++) {
            float value = mClip->getAudioBuffer()->getSample(0, i * (juce::int64) sourceSamplesPerThumbnailSample + j);
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

void AudioThumbnail::setupCallbacks()
{
    connect(mClip->getClipProperties().get(), &Audio::ClipProperties::startOffsetChanged, [=]()
    {
        loadThumbnail(mSourceSamplesPerThumbnailSample);
        emit repaintRequested();
    });
    connect(mClip->getClipProperties().get(), &Audio::ClipProperties::endOffsetChanged, [=]()
    {
        loadThumbnail(mSourceSamplesPerThumbnailSample);
        emit repaintRequested();
    });
}

} // namespace Graphics

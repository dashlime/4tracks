#include "audiothumbnail.h"

namespace Graphics {

AudioThumbnail::AudioThumbnail()
{

}

void AudioThumbnail::loadThumbnail(std::shared_ptr<Audio::AudioClip> clip, int sourceSamplesPerThumbnailSample)
{
    int thumbnailSamples = clip->getAudioBuffer().getNumSamples() / sourceSamplesPerThumbnailSample;

    for (int i = 0; i < thumbnailSamples; i++)
    {
        float min = 0;
        float max = 0;
        for (int j = 0; j < sourceSamplesPerThumbnailSample; j++)
        {
            float value = clip->getAudioBuffer().getSample(0, i*sourceSamplesPerThumbnailSample+j);
            min = value < min ? value : min;
            max = value > max ? value : max;
        }
        mThumbnailValues.push_back(MinMaxValues(min, max));
    }
}

void AudioThumbnail::drawThumbnail(QPainter& p, QRect rect)
{
    int ratio = rect.height() / 2;
    double thumbnailValuesPerPixel = (double)mThumbnailValues.size() / (double)rect.width();

    p.setPen(QPen(QColor("#000000")));
    p.translate(rect.topLeft());
    for (int i = 0; i < rect.width(); i++)
    {
        float min = mThumbnailValues.at(i*thumbnailValuesPerPixel).getMinValue();
        float max = mThumbnailValues.at(i*thumbnailValuesPerPixel).getMaxValue();
        p.drawLine(i, min*ratio + rect.height()/2, i, max*ratio + rect.height()/2);
    }
}

} // namespace Graphics

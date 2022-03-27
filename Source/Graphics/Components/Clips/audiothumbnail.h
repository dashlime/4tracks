#ifndef GRAPHICS_AUDIOTHUMBNAIL_H
#define GRAPHICS_AUDIOTHUMBNAIL_H

#include <QtWidgets>
#include "Audio/Resources/audioresource.h"
#include "Audio/Clip/audioclip.h"

namespace Graphics
{

class AudioThumbnail: public QObject
{
Q_OBJECT

public:
    class MinMaxValues;

    explicit AudioThumbnail(const QSharedPointer<Audio::AudioResource> &resource);

    void loadThumbnail(int sourceSamplesPerThumbnailSample);

    [[nodiscard]] const std::vector<MinMaxValues> &getThumbnailValues() const;

    [[nodiscard]] QSharedPointer<Audio::AudioResource> getAudioResource() const;

private:
    std::vector<MinMaxValues> mThumbnailValues;
    QSharedPointer<Audio::AudioResource> mAudioResource;
};

class AudioThumbnail::MinMaxValues
{
public:
    MinMaxValues(float min, float max)
        : mMin(min), mMax(max)
    {};

    [[nodiscard]] float getMinValue() const
    { return mMin; };
    [[nodiscard]] float getMaxValue() const
    { return mMax; };

private:
    float mMin, mMax;
};

} // namespace Graphics

#endif // GRAPHICS_AUDIOTHUMBNAIL_H

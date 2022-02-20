#ifndef GRAPHICS_AUDIOTHUMBNAIL_H
#define GRAPHICS_AUDIOTHUMBNAIL_H

#include <QtWidgets>
#include "Audio/Clip/audioclip.h"

namespace Graphics
{

class AudioThumbnail: public QObject
{
    Q_OBJECT

public:
    explicit AudioThumbnail(const QSharedPointer<Audio::AudioClip> &clip);

    void loadThumbnail(int sourceSamplesPerThumbnailSample);
    void drawThumbnail(QPainter &p, QRect rect);

    void setupCallbacks();

signals:
    void repaintRequested();

private:
    class MinMaxValues;

    int mSourceSamplesPerThumbnailSample = 0;
    std::vector<MinMaxValues> mThumbnailValues;
    QSharedPointer<Audio::AudioClip> mClip;
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

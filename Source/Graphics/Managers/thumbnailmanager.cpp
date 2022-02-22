#include "thumbnailmanager.h"

namespace Graphics
{

ThumbnailManager::ThumbnailManager()
{}

bool ThumbnailManager::resourceAlreadyLoaded(const QSharedPointer<Audio::AudioResource> &resource)
{
    for (const auto &thumbnail: mAudioThumbnails)
        if (thumbnail->getAudioResource() == resource)
            return true;

    return false;
}

QSharedPointer<AudioThumbnail>
ThumbnailManager::getThumbnailForResource(const QSharedPointer<Audio::AudioResource> &resource) const
{
    for (const auto &thumbnail: mAudioThumbnails)
        if (thumbnail->getAudioResource() == resource)
            return thumbnail;

    return {};
}

void ThumbnailManager::registerAudioResource(const QSharedPointer<Audio::AudioResource> &resource)
{
    if (resourceAlreadyLoaded(resource))
        return;

    auto thumbnail = QSharedPointer<AudioThumbnail>::create(resource);
    thumbnail->loadThumbnail(512);
    mAudioThumbnails.append(thumbnail);
}

QVector<QSharedPointer<AudioThumbnail>> ThumbnailManager::getAudioThumbnails() const
{
    return mAudioThumbnails;
}

void ThumbnailManager::drawThumbnailForClip(const QSharedPointer<Audio::AudioClip> &clip,
                                            QPainter &painter,
                                            QRect rect) const
{
    auto thumbnail = getThumbnailForResource(clip->getAudioResource());
    auto values = thumbnail->getThumbnailValues();

    juce::int64 startOffset = clip->getClipProperties()->getStartOffset() / 512;
    juce::int64 endOffset = clip->getClipProperties()->getEndOffset() / 512;

    float verticalCenter = (float) rect.height() / 2;
    float ratio = verticalCenter;

    double thumbnailValuesPerPixel = (double) (endOffset - startOffset) / (double) rect.width();

    painter.translate(rect.topLeft());
    for (int i = 0; i < rect.width(); i++) {
        float min = values.at(i * thumbnailValuesPerPixel + startOffset).getMinValue();
        float max = values.at(i * thumbnailValuesPerPixel + startOffset).getMaxValue();
        painter.drawLine(i, int(min * ratio + verticalCenter), i, int(max * ratio + verticalCenter));
    }
}

} // Graphics
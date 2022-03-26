#ifndef THUMBNAILMANAGER_H
#define THUMBNAILMANAGER_H

#include "Audio/Resources/audioresource.h"
#include "Graphics/Components/Clips/audiothumbnail.h"

#include <QSharedPointer>

namespace Graphics
{

class ThumbnailManager
{
public:
    ThumbnailManager();

    bool resourceAlreadyLoaded(const QSharedPointer<Audio::AudioResource> &resource);

    [[nodiscard]] QSharedPointer<AudioThumbnail>
    getThumbnailForResource(const QSharedPointer<Audio::AudioResource> &resource) const;

    void registerAudioResource(const QSharedPointer<Audio::AudioResource> &resource);
    void drawThumbnailForClip(const QSharedPointer<Audio::AudioClip> &clip, QPainter &painter, QRect rect) const;

    [[nodiscard]] QVector<QSharedPointer<AudioThumbnail>> getAudioThumbnails() const;

private:
    QVector<QSharedPointer<AudioThumbnail>> mAudioThumbnails;
};

} // Graphics

#endif //THUMBNAILMANAGER_H

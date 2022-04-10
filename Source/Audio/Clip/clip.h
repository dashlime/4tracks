#ifndef CLIP_H
#define CLIP_H

#include <QString>
#include <QPointer>
#include <QSharedPointer>

#include "Audio/JuceIncludes.h"
#include "clipproperties.h"

namespace Audio
{

class Track;

class Clip: public QObject
{
Q_OBJECT

public:
    explicit Clip(const QString &name, Track *parentTrack);

    [[nodiscard]] QSharedPointer<ClipProperties> getClipProperties() const;

    void setReadPosition(juce::int64 newPosition);
    [[nodiscard]] juce::int64 getReadPosition() const;

    enum Type
    {
        AUDIO_CLIP, MIDI_CLIP
    };

    [[nodiscard]] virtual Type getType() const = 0;
    virtual void nextReadPositionChanged() = 0;

signals:
    void dataChanged();

protected:
    QSharedPointer<ClipProperties> mClipProperties;

    juce::int64 mNextReadPosition = 0;
};

}

#endif // CLIP_H

#ifndef VELOCITYPANEL_H
#define VELOCITYPANEL_H

#include <QtWidgets>

#include "Utils/utils.h"
#include "notevelocity.h"
#include "Audio/Clip/midiclip.h"
#include "Audio/Project/project.h"
#include "Audio/Resources/mididata.h"
#include "Graphics/Components/Timeline/timelineproperties.h"
#include "Constants.h"

namespace Graphics
{

class VelocityPanel : public QWidget
{
    Q_OBJECT

public:
    VelocityPanel(const QSharedPointer<Audio::MidiClip>& clip, const QSharedPointer<TimelineProperties>& timelineProperties, QWidget *parent = nullptr);

    void updateNote(const QSharedPointer<Audio::MidiNote>& note);

    void paintEvent(QPaintEvent *) override;

private:
    QSharedPointer<Audio::MidiClip> mClip;
    QSharedPointer<TimelineProperties> mTimelineProperties;

    QVector<QPointer<NoteVelocity>> mVelocityWidgets;
};

}

#endif //VELOCITYPANEL_H

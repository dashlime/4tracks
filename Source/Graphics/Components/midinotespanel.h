#ifndef MIDINOTESPANEL_H
#define MIDINOTESPANEL_H

#include <QtWidgets>

#include "Audio/Clip/midiclip.h"

namespace Graphics
{

class MidiNotesPanel: public QWidget
{
Q_OBJECT
public:
    explicit MidiNotesPanel(const QSharedPointer<Audio::MidiClip>& clip, QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;
private:
    QSharedPointer<Audio::MidiClip> mClip;
};

}

#endif //MIDINOTESPANEL_H

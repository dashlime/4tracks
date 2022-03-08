#ifndef MIDICLIPPANEL_H
#define MIDICLIPPANEL_H

#include <QWidget>
#include "Audio/Project/project.h"
#include "Audio/Clip/midiclip.h"
#include "midieditor.h"

namespace Graphics
{

class MidiClipPanel: public QWidget
{
    Q_OBJECT
public:
    explicit MidiClipPanel(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent = nullptr);

private:
    QSharedPointer<Audio::MidiClip> mClip;
    MidiEditor mMidiEditor;

    QVBoxLayout mMainLayout;
};

} // Graphics

#endif //MIDICLIPPANEL_H

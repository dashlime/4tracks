#ifndef MIDINOTE_H
#define MIDINOTE_H

#include <QtWidgets>

#include "Audio/JuceIncludes.h"
#include "Audio/Clip/midiclip.h"

namespace Graphics
{

class MidiNote: public QWidget
{
    Q_OBJECT

public:
    MidiNote(const juce::MidiMessage& noteOnEvent, const QSharedPointer<Audio::MidiClip>& clip, QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;

private:
    QLabel mNoteLabel;

    QSharedPointer<Audio::MidiClip> mClip;
};

}

#endif //MIDINOTE_H

#ifndef MIDINOTE_H
#define MIDINOTE_H

#include <QtWidgets>

#include "Audio/JuceIncludes.h"
#include "Audio/Clip/midiclip.h"
#include "Graphics/Managers/selection.h"

namespace Graphics
{

class MidiNote: public Selection::SelectableObject
{
    Q_OBJECT

public:
    MidiNote(const QSharedPointer<Audio::MidiNote>& note, const QSharedPointer<Audio::MidiClip>& clip, QWidget *parent = nullptr);

    [[nodiscard]] QSharedPointer<Audio::MidiNote> getMidiNote() const;

    void paintEvent(QPaintEvent *event) override;

    void setSelectedState(bool isSelected) override;
    [[nodiscard]] Type getType() const override;

private:
    QLabel mNoteLabel;

    QSharedPointer<Audio::MidiNote> mNote;
    QSharedPointer<Audio::MidiClip> mClip;
};

}

#endif //MIDINOTE_H

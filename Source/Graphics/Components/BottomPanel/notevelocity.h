#ifndef NOTEVELOCITY_H
#define NOTEVELOCITY_H

#include <QtWidgets>

#include "midinote.h"

namespace Graphics
{

class NoteVelocity: public QWidget
{
    Q_OBJECT

public:
    explicit NoteVelocity(const QSharedPointer<Audio::MidiNote>& correspondingMidiNote, QWidget *parent = nullptr);

    [[nodiscard]] QSharedPointer<Audio::MidiNote> getCorrespondingMidiNote() const;

    void paintEvent(QPaintEvent *event) override;

private:
    QSharedPointer<Audio::MidiNote> mNote;
};

}

#endif //NOTEVELOCITY_H

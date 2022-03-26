#ifndef MIDIDATA_H
#define MIDIDATA_H

#include <QtWidgets>
#include "Audio/JuceIncludes.h"
#include "midinote.h"

namespace Audio
{

class MidiData: public QObject
{
Q_OBJECT

public:

    MidiData();

    void addNote(const QSharedPointer<MidiNote> &note);
    void removeNote(const QSharedPointer<MidiNote> &note);

    [[nodiscard]] QVector<QSharedPointer<MidiNote>> getMidiNotes() const;

signals:
    void noteAdded(const QSharedPointer<MidiNote> &note);
    void noteRemoved(const QSharedPointer<MidiNote> &note);

private:
    QVector<QSharedPointer<MidiNote>> mNotes;
};

}

#endif //MIDIDATA_H

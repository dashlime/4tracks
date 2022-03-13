#ifndef MIDICLIP_H
#define MIDICLIP_H

#include "clip.h"

#include "Audio/Resources/mididata.h"

#include "Constants.h"

namespace Audio
{

class MidiClip: public Clip
{
    Q_OBJECT

public:
    explicit MidiClip(Track *parentTrack);

    [[nodiscard]] QSharedPointer<MidiData> getMidiData() const;

    [[nodiscard]] bool currentScaleContains(int midiNoteNumber) const;
    [[nodiscard]] QString midiNoteName(int midiNoteNumber) const;

    [[nodiscard]] Type getType() const override;

private:
    QSharedPointer<MidiData> mMidiData;
};

}

#endif // MIDICLIP_H

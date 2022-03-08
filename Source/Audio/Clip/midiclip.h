#ifndef MIDICLIP_H
#define MIDICLIP_H

#include "clip.h"

#include "Constants.h"

namespace Audio
{

class MidiClip: public Clip
{
public:
    explicit MidiClip(Track *parentTrack);

    [[nodiscard]] bool currentScaleContains(int midiNoteNumber) const;

    [[nodiscard]] Type getType() const override;
};

}

#endif // MIDICLIP_H

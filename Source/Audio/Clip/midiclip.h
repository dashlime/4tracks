#ifndef MIDICLIP_H
#define MIDICLIP_H

#include "clip.h"

namespace Audio
{

class MidiClip: public Clip
{
public:
    explicit MidiClip(Track *parentTrack);

    [[nodiscard]] Type getType() const override;
};

}

#endif // MIDICLIP_H

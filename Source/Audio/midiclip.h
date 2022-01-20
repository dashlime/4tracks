#ifndef MIDICLIP_H
#define MIDICLIP_H

#include "clip.h"

namespace Audio
{

class MidiClip: public Clip
{
public:
    MidiClip(const std::weak_ptr<Track> &parentTrack);

    int getType() const override;
};

}

#endif // MIDICLIP_H

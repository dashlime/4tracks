#ifndef MIDICLIP_H
#define MIDICLIP_H

#include "clip.h"

namespace Audio {

class MidiClip : public Clip
{
public:
    MidiClip(std::shared_ptr<AudioTrack> parentTrack);

    int getType() const override;
};

}

#endif // MIDICLIP_H

#include "midiclip.h"

namespace Audio
{

MidiClip::MidiClip(std::shared_ptr<Track> parentTrack)
    : Clip(parentTrack)
{

}

int MidiClip::getType() const
{
    return Clip::MIDI_CLIP;
}

}

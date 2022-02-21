#include "midiclip.h"

namespace Audio
{

MidiClip::MidiClip(Track *parentTrack)
    : Clip("Midi Clip", parentTrack)
{}

Clip::Type MidiClip::getType() const
{
    return Clip::MIDI_CLIP;
}

}

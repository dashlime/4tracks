#include "midiclip.h"

namespace Audio
{

MidiClip::MidiClip(const std::weak_ptr<Track> &parentTrack)
    : Clip(parentTrack)
{}

int MidiClip::getType() const
{
    return Clip::MIDI_CLIP;
}

}

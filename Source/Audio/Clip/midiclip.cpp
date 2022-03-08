#include "midiclip.h"

namespace Audio
{

MidiClip::MidiClip(Track *parentTrack)
    : Clip("Midi Clip", parentTrack)
{}

bool MidiClip::currentScaleContains(int midiNoteNumber) const
{
    int note = midiNoteNumber % NOTES_IN_OCTAVE;

    if (MAJOR_SCALE_SCHEME[note])
        return true;

    return false;
}

Clip::Type MidiClip::getType() const
{
    return Clip::MIDI_CLIP;
}

}

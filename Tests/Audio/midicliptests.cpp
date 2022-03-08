#include "audioenginetests.h"

TEST_F(MidiClipTests, CurrentScaleContains)
{
    // test with C-1
    EXPECT_TRUE(mMidiClip->currentScaleContains(0));
    // test with D2
    EXPECT_TRUE(mMidiClip->currentScaleContains(NOTES_IN_OCTAVE * 3 + 2));
    // test with D#2
    EXPECT_FALSE(mMidiClip->currentScaleContains(NOTES_IN_OCTAVE * 3 + 3));
}

TEST_F(MidiClipTests, GetMidiNoteName)
{
    // test with C-1
    EXPECT_TRUE(mMidiClip->midiNoteName(0) == "C-1");
    // test with D2
    EXPECT_TRUE(mMidiClip->midiNoteName(NOTES_IN_OCTAVE * 3 + 2) == "D2");
    // test with D#2
    EXPECT_TRUE(mMidiClip->midiNoteName(NOTES_IN_OCTAVE * 3 + 3) == "D#2");
}
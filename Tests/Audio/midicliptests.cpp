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
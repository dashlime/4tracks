#include "audioenginetests.h"

TEST_F(AudioFileLoaderTests, CheckFileName)
{
    EXPECT_TRUE(mFileLoader->checkFileName("example file.wav"));
    EXPECT_TRUE(mFileLoader->checkFileName("example file.mp3"));
    EXPECT_TRUE(mFileLoader->checkFileName("example file.aiff"));
    EXPECT_FALSE(mFileLoader->checkFileName("example file"));
    EXPECT_FALSE(mFileLoader->checkFileName("example file.txt"));
    EXPECT_FALSE(mFileLoader->checkFileName("example file."));
}
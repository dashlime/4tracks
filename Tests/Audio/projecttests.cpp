#include "audioenginetests.h"

TEST_F(ProjectTests, Play)
{
    mProjectToTest->play();
    EXPECT_TRUE(mProjectToTest->isPlaying());
}

TEST_F(ProjectTests, PlayAndGetNextReadPosition)
{
    mProjectToTest->play();

    auto *buffer = new juce::AudioBuffer<float>();
    auto channelInfo = juce::AudioSourceChannelInfo(buffer, 0, 1024);

    mProjectToTest->addTrack(std::make_shared<Audio::Track>("Test track", 0));
    mProjectToTest->prepareToPlay(1024, DEFAULT_SAMPLE_RATE);
    mProjectToTest->getNextAudioBlock(channelInfo);

    EXPECT_EQ(mProjectToTest->getNextReadPosition(), 1024);

    delete buffer;
}
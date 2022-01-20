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

    mProjectToTest->addTrack(std::make_shared<Audio::Track>("Test track", 0, mProjectToTest));
    mProjectToTest->prepareToPlay(1024, DEFAULT_SAMPLE_RATE);
    mProjectToTest->getNextAudioBlock(channelInfo);

    EXPECT_EQ(mProjectToTest->getNextReadPosition(), 1024);

    delete buffer;
}

TEST_F(ProjectTests, CanRemoveTrack)
{
    mProjectToTest->addTrack(std::make_shared<Audio::Track>("Test track", 0, mProjectToTest));
    EXPECT_FALSE(mProjectToTest->canRemoveTrack());

    mProjectToTest->addTrack(std::make_shared<Audio::Track>("Test track", 1, mProjectToTest));
    EXPECT_TRUE(mProjectToTest->canRemoveTrack());
}

TEST_F(ProjectTests, RemoveTrack)
{
    auto trackToRemove = std::make_shared<Audio::Track>("Test track", 0, mProjectToTest);
    mProjectToTest->addTrack(trackToRemove);
    mProjectToTest->removeTrack(trackToRemove);
    ASSERT_EQ(mProjectToTest->getTracks().size(), 1);

    mProjectToTest->addTrack(std::make_shared<Audio::Track>("Test track", 1, mProjectToTest));
    mProjectToTest->removeTrack(trackToRemove);

    EXPECT_EQ(mProjectToTest->getTracks().size(), 1);
}

TEST_F(ProjectTests, RearrangeTrackIndexes)
{
    mProjectToTest->addTrack(std::make_shared<Audio::Track>("Test track", 1, mProjectToTest));
    mProjectToTest->addTrack(std::make_shared<Audio::Track>("Test track", 4, mProjectToTest));
    mProjectToTest->addTrack(std::make_shared<Audio::Track>("Test track", 2, mProjectToTest));

    mProjectToTest->rearrangeTrackIndexes();
    EXPECT_EQ(mProjectToTest->getTracks().at(0)->getIndex(), 0);
    EXPECT_EQ(mProjectToTest->getTracks().at(1)->getIndex(), 2);
    EXPECT_EQ(mProjectToTest->getTracks().at(2)->getIndex(), 1);
}

TEST_F(ProjectTests, GetTrackByIndex)
{
    auto newTrack = std::make_shared<Audio::Track>("Test track", 2, mProjectToTest);
    mProjectToTest->addTrack(newTrack);
    EXPECT_EQ(mProjectToTest->getTrackByIndex(2).get(), newTrack.get());
}
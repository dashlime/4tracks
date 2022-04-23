#include "audioenginetests.h"

TEST_F(ProjectTests, PlayAndGetNextReadPosition)
{
    auto *buffer = new juce::AudioBuffer<float>(2, 1024);
    auto channelInfo = juce::AudioSourceChannelInfo(buffer, 0, 1024);

    buffer->setSample(0, 0, 1);
    mProjectToTest->getNextAudioBlock(channelInfo);
    EXPECT_EQ(buffer->getSample(0, 0), 0);

    mProjectToTest->play();

    mProjectToTest->createTrack("Test track");
    mProjectToTest->prepareToPlay(1024, DEFAULT_SAMPLE_RATE);
    mProjectToTest->getNextAudioBlock(channelInfo);

    EXPECT_EQ(mProjectToTest->getNextReadPosition(), 1024);

    delete buffer;
}

TEST_F(ProjectTests, Pause)
{
    mProjectToTest->createTrack("Test track");

    mProjectToTest->setNextReadPosition(2000);
    mProjectToTest->play();
    EXPECT_EQ(mProjectToTest->getNextReadPosition(), 2000);
    EXPECT_TRUE(mProjectToTest->isPlaying());

    mProjectToTest->pause();
    EXPECT_EQ(mProjectToTest->getNextReadPosition(), 2000);
    EXPECT_FALSE(mProjectToTest->isPlaying());
}

TEST_F(ProjectTests, Stop)
{
    mProjectToTest->createTrack("Test track");

    mProjectToTest->setNextReadPosition(2000);
    mProjectToTest->play();
    EXPECT_EQ(mProjectToTest->getNextReadPosition(), 2000);
    EXPECT_TRUE(mProjectToTest->isPlaying());

    mProjectToTest->stop();
    EXPECT_EQ(mProjectToTest->getNextReadPosition(), 0);
    EXPECT_FALSE(mProjectToTest->isPlaying());
}

TEST_F(ProjectTests, CanRemoveTrack)
{
    mProjectToTest->createTrack("Test track");
    EXPECT_FALSE(mProjectToTest->canRemoveTrack());

    mProjectToTest->createTrack("Test track");
    EXPECT_TRUE(mProjectToTest->canRemoveTrack());
}

TEST_F(ProjectTests, CanRemoveTracks)
{
    mProjectToTest->createTrack("Test track");
    mProjectToTest->createTrack("Test track");
    EXPECT_FALSE(mProjectToTest->canRemoveTracks(2));

    mProjectToTest->createTrack("Test track");
    EXPECT_TRUE(mProjectToTest->canRemoveTracks(2));
}

TEST_F(ProjectTests, RemoveTrack)
{
    int trackToRemove = mProjectToTest->createTrack("Test track");
    mProjectToTest->createTrack("Test track");

    mProjectToTest->removeTrack(mProjectToTest->getTrackByIndex(trackToRemove));

    EXPECT_EQ(mProjectToTest->getTracks().size(), 1);

    EXPECT_EQ(mProjectToTest->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);
}

TEST_F(ProjectTests, ClearAllTracks)
{
    mProjectToTest->createTrack("Test track");
    mProjectToTest->createTrack("Test track");
    mProjectToTest->createMIDIClip(mProjectToTest->getTrackByIndex(0));
    mProjectToTest->createAudioClip(mProjectToTest->getTrackByIndex(1), "");

    EXPECT_EQ(mProjectToTest->getTracks().size(), 2);
    EXPECT_EQ(mProjectToTest->getClips().size(), 2);

    mProjectToTest->clearAllTracks();

    EXPECT_EQ(mProjectToTest->getTracks().size(), 0);
    EXPECT_EQ(mProjectToTest->getClips().size(), 0);

    EXPECT_EQ(mProjectToTest->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);
}

TEST_F(ProjectTests, RemoveArea)
{
    int track = mProjectToTest->createTrack("Test track");

    int firstClipID = mProjectToTest->createMIDIClip(mProjectToTest->getTrackByIndex(track));
    int secondClipID = mProjectToTest->createMIDIClip(mProjectToTest->getTrackByIndex(track));
    auto firstClip = mProjectToTest->getClips().at(firstClipID);
    auto secondClip = mProjectToTest->getClips().at(secondClipID);

    firstClip->getClipProperties()->setLengthInSamples(1000);
    firstClip->getClipProperties()->setEndOffset(1000);
    secondClip->getClipProperties()->setPositionInSamples(1000);
    secondClip->getClipProperties()->setLengthInSamples(1000);
    secondClip->getClipProperties()->setEndOffset(1000);

    mProjectToTest->removeArea(0, 1, 500, 1000);

    EXPECT_EQ(firstClip->getClipProperties()->getEndOffset(), 500);
    EXPECT_EQ(secondClip->getClipProperties()->getStartOffset(), 500);

    SetUp();
    track = mProjectToTest->createTrack("Test track");

    int clipID = mProjectToTest->createMIDIClip(mProjectToTest->getTrackByIndex(track));
    auto clip = mProjectToTest->getClips().at(clipID);

    clip->getClipProperties()->setLengthInSamples(1000);
    clip->getClipProperties()->setEndOffset(1000);

    mProjectToTest->removeArea(0, 1, 0, 1000);

    EXPECT_EQ(mProjectToTest->getClips().size(), 0);
    EXPECT_EQ(mProjectToTest->getTrackByIndex(track)->getClips().size(), 0);

    SetUp();
    track = mProjectToTest->createTrack("Test track");

    clipID = mProjectToTest->createMIDIClip(mProjectToTest->getTrackByIndex(track));
    clip = mProjectToTest->getClips().at(clipID);

    clip->getClipProperties()->setLengthInSamples(1000);
    clip->getClipProperties()->setEndOffset(1000);

    mProjectToTest->removeArea(0, -1, 600, -100);

    EXPECT_EQ(mProjectToTest->getClips().size(), 2);
    EXPECT_EQ(clip, mProjectToTest->getClips().at(0));

    EXPECT_EQ(clip->getClipProperties()->getStartOffset(), 0);
    EXPECT_EQ(clip->getClipProperties()->getEndOffset(), 500);

    EXPECT_EQ(mProjectToTest->getClips().at(1)->getClipProperties()->getStartOffset(), 600);
    EXPECT_EQ(mProjectToTest->getClips().at(1)->getClipProperties()->getEndOffset(), 1000);

    EXPECT_EQ(mProjectToTest->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);
}

TEST_F(ProjectTests, RearrangeTrackIndexes)
{
    mProjectToTest->addTrack(QSharedPointer<Audio::Track>::create("Test track", 1, mProjectToTest.get()));
    mProjectToTest->addTrack(QSharedPointer<Audio::Track>::create("Test track", 4, mProjectToTest.get()));
    mProjectToTest->addTrack(QSharedPointer<Audio::Track>::create("Test track", 2, mProjectToTest.get()));

    mProjectToTest->rearrangeTrackIndexes();
    EXPECT_EQ(mProjectToTest->getTracks().at(0)->getTrackProperties()->getIndex(), 0);
    EXPECT_EQ(mProjectToTest->getTracks().at(1)->getTrackProperties()->getIndex(), 2);
    EXPECT_EQ(mProjectToTest->getTracks().at(2)->getTrackProperties()->getIndex(), 1);

    EXPECT_EQ(mProjectToTest->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);
}

TEST_F(ProjectTests, GetTrackByIndex)
{
    auto newTrack = QSharedPointer<Audio::Track>::create("Test track", 2, mProjectToTest.get());
    mProjectToTest->addTrack(newTrack);
    EXPECT_EQ(mProjectToTest->getTrackByIndex(2).get(), newTrack.get());
}

TEST_F(ProjectTests, CreateTrack)
{
    int firstIndex = mProjectToTest->createTrack("Test track");
    int secondIndex = mProjectToTest->createTrack("Test track");

    EXPECT_EQ(mProjectToTest->getTracks().size(), 2);
    EXPECT_EQ(firstIndex, 0);
    EXPECT_EQ(secondIndex, 1);

    EXPECT_EQ(mProjectToTest->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);
}

TEST_F(ProjectTests, CreateAudioClip)
{
    EXPECT_EQ(mProjectToTest->createAudioClip(mProjectToTest->getTrackByIndex(0), ""), -1);

    mProjectToTest->createTrack("Test track");
    mProjectToTest->createAudioClip(mProjectToTest->getTrackByIndex(0), "test_path");

    EXPECT_EQ(mProjectToTest->getTrackByIndex(0)->getClips().size(), 1);
    EXPECT_EQ(mProjectToTest->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);

    mProjectToTest->createAudioClip(mProjectToTest->getTrackByIndex(0), "test_path");
    EXPECT_EQ(qSharedPointerDynamicCast<Audio::AudioClip>(mProjectToTest->getClips().at(0))->getAudioResource(),
              qSharedPointerDynamicCast<Audio::AudioClip>(mProjectToTest->getClips().at(1))->getAudioResource());

    mProjectToTest->createMIDIClip(mProjectToTest->getTrackByIndex(0));
    EXPECT_EQ(mProjectToTest->getTrackByIndex(0)->getClips().size(), 2);
}

TEST_F(ProjectTests, CreateMIDIClip)
{
    EXPECT_EQ(mProjectToTest->createMIDIClip(mProjectToTest->getTrackByIndex(0)), -1);

    mProjectToTest->createTrack("Test track");
    mProjectToTest->createMIDIClip(mProjectToTest->getTrackByIndex(0));

    EXPECT_EQ(mProjectToTest->getTrackByIndex(0)->getClips().size(), 1);

    EXPECT_EQ(mProjectToTest->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);
}

TEST_F(ProjectTests, UpdateTotalLength)
{
    mProjectToTest->createTrack("Test track");
    int id = mProjectToTest->createMIDIClip(mProjectToTest->getTrackByIndex(0));
    mProjectToTest->getClips().at(id)->getClipProperties()->setLengthInSamples(1000);

    EXPECT_EQ(mProjectToTest->getTotalLength(), 1000);

    EXPECT_EQ(mProjectToTest->getProjectProperties()->getSavedState(), Audio::ProjectProperties::UNSAVED);
}

TEST_F(ProjectTests, GetAlreadyLoadedResource)
{
    EXPECT_TRUE(mProjectToTest->getAlreadyLoadedResource("").isNull());

    int trackID = mProjectToTest->createTrack("Test track");
    auto track = mProjectToTest->getTrackByIndex(trackID);

    mProjectToTest->createAudioClip(track, "test_file_path");
    EXPECT_FALSE(mProjectToTest->getAlreadyLoadedResource("test_file_path").isNull());
}

TEST_F(ProjectTests, ReleaseResources)
{
    mProjectToTest->play();
    mProjectToTest->releaseResources();

    EXPECT_FALSE(mProjectToTest->isPlaying());
}

TEST_F(ProjectTests, DuplicateClip)
{
    int trackID = mProjectToTest->createTrack("Test track");
    auto track = mProjectToTest->getTrackByIndex(trackID);

    mProjectToTest->createAudioClip(track, "test_file_path");
    mProjectToTest->getClips().at(0)->getClipProperties()->setEndOffset(10);
    mProjectToTest->duplicateClip(mProjectToTest->getClips().at(0));

    EXPECT_EQ(mProjectToTest->getClips().size(), 2);
    EXPECT_EQ(qSharedPointerDynamicCast<Audio::AudioClip>(mProjectToTest->getClips().at(0))->getAudioResource(),
              qSharedPointerDynamicCast<Audio::AudioClip>(mProjectToTest->getClips().at(1))->getAudioResource());
    EXPECT_EQ(mProjectToTest->getClips().at(1)->getClipProperties()->getPositionInSamples(), 10);
    EXPECT_EQ(mProjectToTest->getClips().at(1)->getClipProperties()->getEndOffset(), 10);

    ProjectTests::SetUp();

    trackID = mProjectToTest->createTrack("Test track");
    track = mProjectToTest->getTrackByIndex(trackID);
    mProjectToTest->createMIDIClip(track);

    auto midiClip = qSharedPointerDynamicCast<Audio::MidiClip>(mProjectToTest->getClips().at(0));
    auto testNote = QSharedPointer<Audio::MidiNote>::create(10, juce::MidiMessage());
    midiClip->getMidiData()->addNote(testNote);

    mProjectToTest->duplicateClip(mProjectToTest->getClips().at(0));
    ASSERT_EQ(mProjectToTest->getClips().size(), 2);
    ASSERT_EQ(qSharedPointerDynamicCast<Audio::MidiClip>(mProjectToTest->getClips().at(1))->getMidiData()->getMidiNotes().size(), 1);
    EXPECT_EQ(midiClip->getMidiData()->getMidiNotes().at(0),
              qSharedPointerDynamicCast<Audio::MidiClip>(mProjectToTest->getClips().at(1))->getMidiData()->getMidiNotes().at(0));
}
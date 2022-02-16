#include "audioenginetests.h"

void TrackTests::SetUp()
{
    mProjectToTest.reset(new Audio::Project("Test project"));
    mTrackToTest = mProjectToTest->getTrackByIndex(mProjectToTest->createTrack("Test track"));
}

void TrackTests::TearDown()
{}

void ProjectTests::SetUp()
{
    mProjectToTest = QSharedPointer<Audio::Project>::create("Test project");
}

void ProjectTests::TearDown()
{}

void AudioFileLoaderTests::SetUp()
{
    mFileLoader.reset(new Audio::AudioFileLoader());
}

void AudioFileLoaderTests::TearDown()
{}

void AudioClipTests::SetUp()
{
    mParentProject = QSharedPointer<Audio::Project>::create("Test project");
    mParentTrack = mParentProject->getTrackByIndex(mParentProject->createTrack("Test track"));

    int clipID = mParentProject->createAudioClip(mParentTrack, "");
    mAudioClip = qSharedPointerCast<Audio::AudioClip>(mParentProject->getClips().at(clipID));
}

void AudioClipTests::TearDown()
{}

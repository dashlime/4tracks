#include "gtest/gtest.h"

#include "Audio/Project/project.h"
#include "Audio/Utils/audiofileloader.h"
#include "Utils/projectsaver.h"

class TrackTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    QSharedPointer<Audio::Track> mTrackToTest;
    QScopedPointer<Audio::Project> mProjectToTest;
};

class ProjectTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    QSharedPointer<Audio::Project> mProjectToTest;
};

class AudioFileLoaderTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    QScopedPointer<Audio::AudioFileLoader> mFileLoader;
};

class AudioClipTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    QSharedPointer<Audio::AudioClip> mAudioClip;
    QSharedPointer<Audio::Track> mParentTrack;
    QSharedPointer<Audio::Project> mParentProject;
};

class MidiClipTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    QSharedPointer<Audio::MidiClip> mMidiClip;
    QSharedPointer<Audio::Track> mParentTrack;
    QSharedPointer<Audio::Project> mParentProject;
};

class AudioProjectSaverTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    QSharedPointer<Audio::Project> mProject;
    QScopedPointer<ProjectSaver> mProjectSaver;
};
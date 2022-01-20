#include "gtest/gtest.h"

#include "Audio/project.h"

class TrackTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Audio::Track> mTrackToTest;
};

class ProjectTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<Audio::Project> mProjectToTest;
};
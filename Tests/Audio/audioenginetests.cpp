#include "audioenginetests.h"

void TrackTests::SetUp()
{
    mTrackToTest = std::make_shared<Audio::Track>("Test track", 0, std::make_shared<Audio::Project>("Test project"));
}
void TrackTests::TearDown()
{
}
void ProjectTests::SetUp()
{
    mProjectToTest = std::make_shared<Audio::Project>("Test project");
}
void ProjectTests::TearDown()
{
}

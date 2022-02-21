#include "uitests.h"

TEST_F(ThumbnailManagerTests, ResourceAlreadyLoaded)
{
    auto resource = QSharedPointer<Audio::AudioResource>::create(QSharedPointer<juce::AudioSampleBuffer>::create(), "");
    EXPECT_FALSE(mThumbnailManager->resourceAlreadyLoaded(resource));

    mThumbnailManager->registerAudioResource(resource);

    EXPECT_TRUE(mThumbnailManager->resourceAlreadyLoaded(resource));
}
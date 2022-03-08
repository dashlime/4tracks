#include "uitests.h"

TEST_F(DivisionsMarkerTests, BeatToString)
{
    EXPECT_TRUE(Graphics::DivisionsMarker::beatToString(0) == "1");
    EXPECT_TRUE(Graphics::DivisionsMarker::beatToString(4) == "2");
    EXPECT_TRUE(Graphics::DivisionsMarker::beatToString(2) == "1.3");
    EXPECT_TRUE(Graphics::DivisionsMarker::beatToString(2.5) == "1.3.3");
    EXPECT_TRUE(Graphics::DivisionsMarker::beatToString(2.75) == "1.3.4");
    EXPECT_TRUE(Graphics::DivisionsMarker::beatToString(2.875) == "1.3.4.3");
}
#include "uitests.h"

void SelectionManagerTests::SetUp()
{
    mSelectionManager.reset(new Graphics::SelectionManager());
}

void SelectionManagerTests::TearDown()
{}

void ThumbnailManagerTests::SetUp()
{
    mThumbnailManager.reset(new Graphics::ThumbnailManager());
}

void ThumbnailManagerTests::TearDown()
{}

void DivisionsMarkerTests::SetUp()
{}

void DivisionsMarkerTests::TearDown()
{}

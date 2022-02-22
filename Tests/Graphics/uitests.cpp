#include "uitests.h"

void SelectionManagerTests::SetUp()
{
    mSelectionManager.reset(new Graphics::Selection());
}

void SelectionManagerTests::TearDown()
{}

void ThumbnailManagerTests::SetUp()
{
    mThumbnailManager.reset(new Graphics::ThumbnailManager());
}

void ThumbnailManagerTests::TearDown()
{}

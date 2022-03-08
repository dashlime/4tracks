#ifndef UITESTS_H
#define UITESTS_H

#include "gtest/gtest.h"

#include <QScopedPointer>
#include <QSharedPointer>
#include "Graphics/Managers/selection.h"
#include "Graphics/Managers/thumbnailmanager.h"
#include "Graphics/Components/divisionsmarker.h"

class SelectionManagerTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    QScopedPointer<Graphics::Selection> mSelectionManager;
};

class ThumbnailManagerTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    QScopedPointer<Graphics::ThumbnailManager> mThumbnailManager;
};

class DivisionsMarkerTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;
};


#endif
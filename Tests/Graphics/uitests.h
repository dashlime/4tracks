#ifndef UITESTS_H
#define UITESTS_H

#include "gtest/gtest.h"

#include <QScopedPointer>
#include "Graphics/Managers/selection.h"

class SelectionManagerTests: public ::testing::Test
{
public:
    void SetUp() override;
    void TearDown() override;

    QScopedPointer<Graphics::Selection> mSelectionManager;
};


#endif
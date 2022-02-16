#include "uitests.h"

TEST_F(SelectionManagerTests, GenerateSelectionModifiers)
{
    auto *event = new QMouseEvent(QMouseEvent::MouseButtonPress, QPoint(), Qt::LeftButton, Qt::MouseButtons(), Qt::KeyboardModifiers());
    auto modifiers = Graphics::Selection::generateSelectionModifiers(event);
    EXPECT_FALSE(modifiers.testFlag(Graphics::Selection::CtrlModifier));
    EXPECT_FALSE(modifiers.testFlag(Graphics::Selection::ShiftModifier));
    EXPECT_FALSE(modifiers.testFlag(Graphics::Selection::RightButtonModifier));

    event = new QMouseEvent(QMouseEvent::MouseButtonPress, QPoint(), Qt::LeftButton, Qt::MouseButtons(), Qt::KeyboardModifiers(Qt::ControlModifier));
    modifiers = Graphics::Selection::generateSelectionModifiers(event);
    EXPECT_TRUE(modifiers.testFlag(Graphics::Selection::CtrlModifier));
    EXPECT_FALSE(modifiers.testFlag(Graphics::Selection::ShiftModifier));
    EXPECT_FALSE(modifiers.testFlag(Graphics::Selection::RightButtonModifier));

    delete event;
}

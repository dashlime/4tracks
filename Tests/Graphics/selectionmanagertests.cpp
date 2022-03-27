#include "uitests.h"

TEST_F(SelectionManagerTests, GenerateSelectionModifiers)
{
    auto *event = new QMouseEvent(QMouseEvent::MouseButtonPress,
                                  QPoint(),
                                  Qt::LeftButton,
                                  Qt::MouseButtons(),
                                  Qt::KeyboardModifiers());
    auto modifiers = Graphics::SelectionManager::generateSelectionModifiers(event);
    EXPECT_FALSE(modifiers.testFlag(Graphics::SelectionManager::CtrlModifier));
    EXPECT_FALSE(modifiers.testFlag(Graphics::SelectionManager::ShiftModifier));
    EXPECT_FALSE(modifiers.testFlag(Graphics::SelectionManager::RightButtonModifier));

    event = new QMouseEvent(QMouseEvent::MouseButtonPress,
                            QPoint(),
                            Qt::LeftButton,
                            Qt::MouseButtons(),
                            Qt::KeyboardModifiers(Qt::ControlModifier));
    modifiers = Graphics::SelectionManager::generateSelectionModifiers(event);
    EXPECT_TRUE(modifiers.testFlag(Graphics::SelectionManager::CtrlModifier));
    EXPECT_FALSE(modifiers.testFlag(Graphics::SelectionManager::ShiftModifier));
    EXPECT_FALSE(modifiers.testFlag(Graphics::SelectionManager::RightButtonModifier));

    delete event;
}
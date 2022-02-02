#ifndef GRAPHICS_POSITIONBAROVERLAY_H
#define GRAPHICS_POSITIONBAROVERLAY_H

#include <QtWidgets>

namespace Graphics
{

class PositionBarOverlay: public QWidget
{
Q_OBJECT
public:
    explicit PositionBarOverlay(QWidget *parent = nullptr);

    void barPositionChanged(int newPosition);

    void paintEvent(QPaintEvent *) override;

signals:
private:
    int mPosition = 0;
};

} // namespace Graphics

#endif // GRAPHICS_POSITIONBAROVERLAY_H

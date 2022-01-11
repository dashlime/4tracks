#ifndef GRAPHICS_POSITIONBAROVERLAY_H
#define GRAPHICS_POSITIONBAROVERLAY_H

#include <QtWidgets>

namespace Graphics {

class PositionBarOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit PositionBarOverlay(QWidget *parent = nullptr);

    void barPositionChanged(double newPosition);

    void paintEvent(QPaintEvent *) override;

signals:
private:
    double mPosition = 0;
};

} // namespace Graphics

#endif // GRAPHICS_POSITIONBAROVERLAY_H

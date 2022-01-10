#ifndef GRAPHICS_DIVISIONSMARKER_H
#define GRAPHICS_DIVISIONSMARKER_H

#include <QtWidgets>

namespace Graphics {

class DivisionsMarker : public QWidget
{
    Q_OBJECT
public:
    explicit DivisionsMarker(double division, double pixelsPerBeat, QWidget *parent = nullptr);

    void refresh(double newDivision, double pixelsPerBeat);

    void paintEvent(QPaintEvent *) override;
signals:
private:
    double mDivision;
    double mPixelsPerBeat;
};

} // namespace Graphics

#endif // GRAPHICS_DIVISIONSMARKER_H

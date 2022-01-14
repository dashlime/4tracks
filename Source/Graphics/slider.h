#ifndef GRAPHICS_SLIDER_H
#define GRAPHICS_SLIDER_H

#include <QtWidgets>

namespace Graphics
{

class Slider: public QAbstractSlider
{
Q_OBJECT
public:
    Slider(QWidget *parent = nullptr);

    QLabel *getValueLabel();

    void sliderChange(QAbstractSlider::SliderChange change) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLabel mLabel;
};

} // namespace Graphics

#endif // GRAPHICS_SLIDER_H

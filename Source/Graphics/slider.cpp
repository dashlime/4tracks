#include "slider.h"

namespace Graphics {

Slider::Slider(QWidget *parent) : QAbstractSlider(parent)
{
    setStyleSheet("background-color: #D4D4D8;");

    mLabel.setText("0");
    mLabel.setStyleSheet("color: black; font-weight: 500; font-size: 9pt; margin: 0; padding: 0; background: none;");

    setMinimumSize(20, 20);
    setOrientation(Qt::Vertical);
    sliderChange(SliderOrientationChange);
}

QLabel* Slider::getValueLabel()
{
    return &mLabel;
}

void Slider::sliderChange(QAbstractSlider::SliderChange change)
{
    switch (change) {
    case SliderRangeChange:
        break;

    case SliderOrientationChange:
        if (layout() != nullptr)
            layout()->removeWidget(&mLabel);

        if (orientation() == Qt::Vertical)
        {
            setLayout(new QVBoxLayout());

            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
            updateGeometry();
        }
        else if (orientation() == Qt::Horizontal)
        {
            setLayout(new QHBoxLayout());

            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            updateGeometry();
        }

        layout()->setContentsMargins(0,0,0,0);
        layout()->setSpacing(0);
        layout()->addWidget(&mLabel);
        layout()->setAlignment(&mLabel, Qt::AlignCenter);
        break;

    case SliderStepsChange:
        break;

    case SliderValueChange:
        mLabel.setText(QString::number(value()));
        emit valueChanged(value());
        break;
    }
}

void Slider::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    if (orientation() == Qt::Vertical)
        p.fillRect(0, height(), width(), -double(height())/double(maximum() - minimum())*value(), QColor("#A1A1AA"));

    else if (orientation() == Qt::Horizontal)
        p.fillRect(0, 0, double(width())/double(maximum() - minimum())*value(), height(), QColor("#A1A1AA"));
}

void Slider::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (orientation() == Qt::Vertical)
        setValue((height()-event->y())*(double(maximum() - minimum())/double(height())));

    else if (orientation() == Qt::Horizontal)
        setValue(event->x()*(double(maximum() - minimum())/double(width())));

    repaint();
}

} // namespace Graphics

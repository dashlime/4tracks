#ifndef GRAPHICS_SELECTIONOVERLAY_H
#define GRAPHICS_SELECTIONOVERLAY_H

#include <QtWidgets>

namespace Graphics
{

class SelectionOverlay: public QWidget
{
Q_OBJECT
public:
    explicit SelectionOverlay(QWidget *parent = nullptr);

    bool areaContains(QPoint point) const;
    void areaChanged(QRect selectionArea);

    void paintEvent(QPaintEvent *event) override;

signals:
private:
    QRect mSelectionArea;
};

} // namespace Graphics

#endif // GRAPHICS_SELECTIONOVERLAY_H

#ifndef GRAPHICS_CLIPSGRID_H
#define GRAPHICS_CLIPSGRID_H

#include "Audio/project.h"
#include "graphics_clip.h"
#include "utils_functions.h"
#include "positionbaroverlay.h"

#include <QWidget>
#include <QStyleOption>
#include <QLayout>
#include <QPainter>
#include <QTimer>

namespace Graphics {

class ClipsGrid : public QWidget
{
    Q_OBJECT
public:
    explicit ClipsGrid(QWidget *parent = nullptr);

    void refreshBpm(double bpm);
    void setProject(std::shared_ptr<Audio::Project> project);

    void refreshClips();

    void refreshZoomLevel(double newZoomLevel);
    double getZoomLevel() const;

    double getDivision() const;

    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

public slots:
    void drawPositionBar();

private:
    double mBpm;

    std::shared_ptr<Audio::Project> mAudioTimeline;
    double mPixelsPerBeat;

    double mZoomLevel = 1.f;

    PositionBarOverlay mPositionBarWidget;

    QTimer mPositionBarTimer;

    QVector<std::shared_ptr<Clip>> mClips;
};

} // namespace Graphics

#endif // GRAPHICS_CLIPSGRID_H

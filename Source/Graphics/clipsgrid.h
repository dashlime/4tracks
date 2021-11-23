#ifndef GRAPHICS_CLIPSGRID_H
#define GRAPHICS_CLIPSGRID_H

#include "Audio/timeline.h"
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
    void setAudioTimeline(std::shared_ptr<Audio::Timeline> audioTimeline);

    void refreshClips();

    void refreshZoomLevel(double newZoomLevel);
    double getZoomLevel() const;

    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void drawPositionBar();

private:
    double mBpm;

    std::shared_ptr<Audio::Timeline> mAudioTimeline;
    double mPixelsPerBeat;

    double mZoomLevel = 1.f;

    PositionBarOverlay mPositionBarWidget;

    QTimer mPositionBarTimer;

    QVector<std::shared_ptr<Clip>> mClips;
};

} // namespace Graphics

#endif // GRAPHICS_CLIPSGRID_H

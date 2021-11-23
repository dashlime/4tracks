#ifndef GRAPHICS_TIMELINE_H
#define GRAPHICS_TIMELINE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include "track.h"
#include "clipsgrid.h"
#include "../Audio/timeline.h"
#include "../Audio/clip.h"

namespace Graphics {
    class Timeline;
}

class Graphics::Timeline : public QWidget
{
    Q_OBJECT
public:
    Timeline(QWidget *parent = nullptr);

    void setAudioTimeline(std::shared_ptr<Audio::Timeline> timeline);
    void displayTracks();

    void refreshZoomLevel(double newZoomLevel);
    double getZoomLevel() const;

    void refreshClipsGridGeometry();
    void refreshBpm();

    void resizeEvent(QResizeEvent *event) override;

signals:

private:
    std::shared_ptr<Audio::Timeline> mAudioTimeline;
    QScrollArea mScrollArea;
    Graphics::ClipsGrid mClipsGrid;
    QGridLayout mMainLayout;

    double mZoomLevel = 1.f;
};

#endif // TIMELINE_H

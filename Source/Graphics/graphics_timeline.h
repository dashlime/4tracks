#ifndef GRAPHICS_TIMELINE_H
#define GRAPHICS_TIMELINE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include "track.h"
#include "clipsgrid.h"
#include "../Audio/project.h"
#include "../Audio/clip.h"
#include "utils_functions.h"

namespace Graphics {
    class Timeline;
}

class Graphics::Timeline : public QWidget
{
    Q_OBJECT
public:
    Timeline(QWidget *parent = nullptr);

    void setProject(std::shared_ptr<Audio::Project> project);
    void displayTracks();

    void refreshZoomLevel(double newZoomLevel);
    double getZoomLevel() const;

    void refreshClipsGridGeometry();
    void refreshBpm();

    double getDivision() const;

    void resizeEvent(QResizeEvent *event) override;

signals:

private:
    std::shared_ptr<Audio::Project> mProject;
    QScrollArea mScrollArea;
    Graphics::ClipsGrid mClipsGrid;
    QGridLayout mMainLayout;

    double mZoomLevel = 1.f;
};

#endif // TIMELINE_H

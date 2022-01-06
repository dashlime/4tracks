#ifndef GRAPHICS_TIMELINE_H
#define GRAPHICS_TIMELINE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QStyleOption>
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

    void refreshBpm();

    double getDivision() const;

    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

signals:

private:
    std::shared_ptr<Audio::Project> mProject;
    Graphics::ClipsGrid mClipsGrid;
    QGridLayout mMainLayout;
    QPushButton mAddTrackButton;
    QWidget mSpacerWidget;

    QVector<Track *> mTracks;

    double mZoomLevel = 1.f;
};

#endif // TIMELINE_H

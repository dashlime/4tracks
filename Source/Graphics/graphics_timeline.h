#ifndef GRAPHICS_TIMELINE_H
#define GRAPHICS_TIMELINE_H

#include <QtWidgets>
#include <QtUiPlugin/QDesignerExportWidget>
#include <vector>

#include "track.h"
#include "selection.h"
#include "clipsgrid.h"
#include "../Audio/project.h"
#include "../Audio/clip.h"
#include "utils_functions.h"
#include "divisionsmarker.h"

namespace Graphics {
    class Timeline;
}

class QDESIGNER_WIDGET_EXPORT Graphics::Timeline : public QWidget
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
    void mousePressEvent(QMouseEvent *event) override;

signals:

private:
    std::shared_ptr<Audio::Project> mProject;
    std::unique_ptr<ClipsGrid> mClipsGrid;
    QVBoxLayout mMainLayout;
    QVBoxLayout mTracksLayout;
    QHBoxLayout mScrollLayout;
    QHBoxLayout mDivisionsMarkerLayout;
    QPushButton mAddTrackButton;
    QWidget mTracksWidget;
    QWidget mScrollWidget;
    QWidget mDivisionsMarkerWidget;

    QScrollArea mVerticalScrollView;

    std::unique_ptr<DivisionsMarker> mDivisionsMarker;

    QWidget mTracksSpacer;
    QWidget mDivisionsMarkerSpacer;

    std::shared_ptr<Selection> mCurrentSelection;

    std::vector<std::shared_ptr<Track>> mTracks;

    double mZoomLevel = 1.f;
};

#endif // TIMELINE_H

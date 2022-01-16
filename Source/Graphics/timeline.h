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
#include "Utils/utils_functions.h"
#include "divisionsmarker.h"

namespace Graphics
{
class Timeline;
}

class QDESIGNER_WIDGET_EXPORT Graphics::Timeline: public QWidget
{
Q_OBJECT
public:
    Timeline(QWidget *parent = nullptr);

    void initializeLayouts();

    void setProject(std::shared_ptr<Audio::Project> project);
    void displayTracks();

    void refreshZoomLevel(double newZoomLevel);
    double getZoomLevel() const;

    void refreshBpm();

    double getDivision() const;

    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:

private:
    std::shared_ptr<Selection> mCurrentSelection;

    QScrollArea mVerticalScrollView;
    QWidget mScrollWidget;

    std::shared_ptr<Audio::Project> mProject;
    QPointer<ClipsGrid> mClipsGrid;

    QWidget mDivisionsMarkerWidget;
    QWidget mDivisionsMarkerSpacer;
    QPointer<DivisionsMarker> mDivisionsMarker;

    QWidget mTracksWidget;
    QWidget mTracksSpacer;
    std::vector<QPointer<Track>> mTracks;

    QVBoxLayout mMainLayout;
    QVBoxLayout mTracksLayout;
    QHBoxLayout mScrollLayout;
    QHBoxLayout mDivisionsMarkerLayout;
    QPushButton mAddTrackButton;

    double mZoomLevel = 1.f;
};

#endif // TIMELINE_H

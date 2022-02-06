#ifndef GRAPHICS_TIMELINE_H
#define GRAPHICS_TIMELINE_H

#include <QtWidgets>
#include <QtUiPlugin/QDesignerExportWidget>
#include <vector>

#include "track.h"
#include "Graphics/Managers/selection.h"
#include "clipsgrid.h"
#include "Audio/Project/project.h"
#include "Audio/Clip/clip.h"
#include "Utils/utils_functions.h"
#include "divisionsmarker.h"
#include "Graphics/Managers/timelineproperties.h"

namespace Graphics
{

class QDESIGNER_WIDGET_EXPORT Timeline: public QWidget
{
Q_OBJECT
public:
    Timeline(const QSharedPointer<Audio::Project> &project, QWidget *parent = nullptr);

    [[nodiscard]] QSharedPointer<TimelineProperties> getTimelineProperties() const;
    [[nodiscard]] QPointer<ClipsGrid> getClipsGrid() const;

    void setupComponents();
    void setupLayouts();

    void setupCallbacks();

    void setNewScrollPosition(int scrollPosInPixels);

    int getClipsGridWidth() const;

    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:

private:
    QSharedPointer<TimelineProperties> mTimelineProperties;

    double mZoomLevel = 1.f;

    QScrollArea mVerticalScrollView;
    QWidget mScrollWidget;

    QSharedPointer<Audio::Project> mProject;
    QPointer<ClipsGrid> mClipsGrid;
    QWidget mClipsGridViewport;

    QWidget mDivisionsMarkerWidget;
    QWidget mDivisionsMarkerSpacer;
    QPointer<DivisionsMarker> mDivisionsMarker;
    QWidget mDivisionsMarkerViewport;

    QWidget mTracksWidget;
    QWidget mTracksSpacer;
    QVector<QPointer<Track>> mTracks;

    QVBoxLayout mMainLayout;
    QVBoxLayout mTracksLayout;
    QHBoxLayout mScrollLayout;
    QHBoxLayout mDivisionsMarkerLayout;
    QPushButton mAddTrackButton;
};

}

#endif // GRAPHICS_TIMELINE_H

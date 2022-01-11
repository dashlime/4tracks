#ifndef GRAPHICS_CLIPSGRID_H
#define GRAPHICS_CLIPSGRID_H

#include <QtWidgets>

#include "Audio/project.h"
#include "clip.h"
#include "Utils/utils_functions.h"
#include "positionbaroverlay.h"
#include "selectionoverlay.h"
#include "selection.h"

namespace Graphics {

class ClipsGrid : public QWidget, public Selection::Callback
{
    Q_OBJECT
public:
    explicit ClipsGrid(std::shared_ptr<Selection> currentSelection, QWidget *parent = nullptr);

    void setProject(std::shared_ptr<Audio::Project> project);

    void refreshBpm(double bpm);
    void refreshTracks();
    void refreshClipsGeometry();

    void refreshZoomLevel(double newZoomLevel);
    double getZoomLevel() const;

    double getPixelsPerBeat() const;
    double getDivision() const;

    int roundPosition(int positionInSamples) const;

    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void selectionChanged() override;

    std::function<void()> onDivisionChanged;

public slots:
    void drawPositionBar();

private:
    double mBpm;
    double mZoomLevel = 1.f;
    double mPixelsPerBeat = 0;

    Clip* mMovingClip = nullptr;
    QPoint clickPosition;

    std::shared_ptr<Selection> mCurrentSelection;
    std::shared_ptr<Audio::Project> mProject;

    PositionBarOverlay mPositionBarWidget;
    QTimer mPositionBarTimer;
    SelectionOverlay mSelectionOverlay;

    std::vector<std::shared_ptr<Clip>> mClips;
};

} // namespace Graphics

#endif // GRAPHICS_CLIPSGRID_H

#ifndef GRAPHICS_CLIPSGRID_H
#define GRAPHICS_CLIPSGRID_H

#include <QtWidgets>

#include "Audio/Project/project.h"
#include "clip.h"
#include "Utils/utils_functions.h"
#include "Graphics/Overlays/positionbaroverlay.h"
#include "Graphics/Overlays/selectionoverlay.h"
#include "Graphics/Managers/selection.h"
#include "Graphics/Managers/timelineproperties.h"

namespace Graphics
{

class ClipsGrid: public QWidget
{
Q_OBJECT
public:
    explicit ClipsGrid(const QSharedPointer<Audio::Project> &project,
                       const QSharedPointer<TimelineProperties> &timelineProperties,
                       QWidget *parent = nullptr);

    void setupCallbacks();
    void setupTrackCallbacks(const QPointer<Audio::Track> &track);
    void setupClipCallbacks(const QPointer<Audio::Clip> &clip);

    void updateClipsGeometry();
    void updateSelectionOverlay();

    [[nodiscard]] double getDivision() const;

    [[nodiscard]] int roundPosition(int positionInSamples) const;

    [[nodiscard]] int samplesToPixels(int samples) const;
    [[nodiscard]] int pixelsToSamples(int pixels) const;

    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void drawPositionBar();

private:
    Clip *mMovingClip = nullptr;
    QPoint clickPosition;

    QSharedPointer<TimelineProperties> mTimelineProperties;
    QSharedPointer<Audio::Project> mProject;

    PositionBarOverlay mPositionBarWidget;
    QTimer mPositionBarTimer;
    SelectionOverlay mSelectionOverlay;

    QVector<QPointer<Clip>> mClips;
};

} // namespace Graphics

#endif // GRAPHICS_CLIPSGRID_H

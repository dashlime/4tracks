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
#include "Graphics/Managers/thumbnailmanager.h"

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
    void setupClipCallbacks(const QPointer<Audio::Clip> &clip);

    void updateClipsGeometry();
    void updateSelectionOverlay();
    void updateMinimumSize();

    [[nodiscard]] double getDivision() const;

    [[nodiscard]] juce::int64 roundPosition(juce::int64 positionInSamples) const;

    [[nodiscard]] int samplesToPixels(juce::int64 samples) const;
    [[nodiscard]] juce::int64 pixelsToSamples(int pixels) const;

    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void drawPositionBar();

signals:
    void sizeChanged();

private:
    Clip *mMovingClip = nullptr;
    QPoint clickPosition;

    QSharedPointer<ThumbnailManager> mThumbnailManager;
    QSharedPointer<TimelineProperties> mTimelineProperties;
    QSharedPointer<Audio::Project> mProject;

    PositionBarOverlay mPositionBarWidget;
    QTimer mPositionBarTimer;
    SelectionOverlay mSelectionOverlay;

    QVector<QPointer<Clip>> mClips;
};

} // namespace Graphics

#endif // GRAPHICS_CLIPSGRID_H

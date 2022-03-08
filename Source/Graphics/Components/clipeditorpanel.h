#ifndef CLIPEDITORPANEL_H
#define CLIPEDITORPANEL_H

#include <QWidget>

#include "Graphics/Managers/timelineproperties.h"
#include "clip.h"
#include "midiclippanel.h"

namespace Graphics
{

class ClipEditorPanel: public QWidget
{
    Q_OBJECT
public:
    explicit ClipEditorPanel(const QSharedPointer<TimelineProperties> &properties, QWidget *parent = nullptr);

    void setupLayouts();
    void setupComponents();

    void updateUI();

    void paintEvent(QPaintEvent *) override;
private:
    QSharedPointer<TimelineProperties> mTimelineProperties;

    QHBoxLayout mMainLayout;

    QWidget mClipPropertiesWidget;
    QVBoxLayout mClipPropertiesLayout;

    ElidedLabel mClipNameLabel;
    QPointer<MidiClipPanel> mMidiClipPanel;
};

}

#endif //CLIPEDITORPANEL_H

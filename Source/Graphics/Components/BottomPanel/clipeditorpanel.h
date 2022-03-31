#ifndef CLIPEDITORPANEL_H
#define CLIPEDITORPANEL_H

#include <QWidget>

#include "Graphics/Components/Timeline/timelineproperties.h"
#include "Graphics/Components/Clips/clip.h"
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
    QPointer<MidiClipPanel> getAlreadyLoadedMidiClipPanel(const QSharedPointer<Audio::MidiClip>& clip);

    QSharedPointer<TimelineProperties> mTimelineProperties;

    QHBoxLayout mMainLayout;

    QWidget mClipPropertiesWidget;
    QVBoxLayout mClipPropertiesLayout;

    ElidedLabel mClipNameLabel;
    QVector<QPointer<MidiClipPanel>> mMidiClipPanels;

    QPointer<MidiClipPanel> mCurrentPanel;
};

}

#endif //CLIPEDITORPANEL_H

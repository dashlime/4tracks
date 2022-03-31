#ifndef MIDICLIPPANEL_H
#define MIDICLIPPANEL_H

#include <QWidget>
#include "Audio/Project/project.h"
#include "Audio/Clip/midiclip.h"
#include "midinotespanel.h"
#include "midieditor.h"

namespace Graphics
{

class MidiClipPanel: public QWidget
{
Q_OBJECT
public:
    explicit MidiClipPanel(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent = nullptr);

    [[nodiscard]] QSharedPointer<Audio::MidiClip> getAssociatedClip() const;

    void paintEvent(QPaintEvent *event) override;

private:
    QSharedPointer<Audio::MidiClip> mClip;
    QPointer<MidiEditor> mMidiEditor;
    QPointer<MidiNotesPanel> mMidiNotesPanel;

    QPointer<QWidget> mVerticalScrollWidget;
    QHBoxLayout mVerticalScrollLayout;

    QWidget mDivisionsMarkerContainer;
    QHBoxLayout mDivisionsMarkerLayout;
    QPointer<DivisionsMarker> mDivisionsMarker;

    QPointer<QScrollArea> mVerticalScrollArea;
    QVBoxLayout mMainLayout;
};

} // Graphics

#endif //MIDICLIPPANEL_H

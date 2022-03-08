#include "midiclippanel.h"

namespace Graphics
{

MidiClipPanel::MidiClipPanel(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent) : QWidget(parent), mClip(clip), mMidiEditor(clip)
{
    setLayout(&mMainLayout);
    mMainLayout.addWidget(&mMidiEditor);
    mMainLayout.setContentsMargins(0, 0, 0, 0);
}

} // Graphics
#include "velocitypanel.h"

namespace Graphics
{

VelocityPanel::VelocityPanel(const QSharedPointer<Audio::MidiClip> &clip, const QSharedPointer<TimelineProperties>& timelineProperties, QWidget *parent)
    : QWidget(parent), mClip(clip), mTimelineProperties(timelineProperties)
{
    connect(mClip->getMidiData().get(), &Audio::MidiData::noteDataChanged, [=](const QSharedPointer<Audio::MidiNote>& changedNote) {
        updateNote(changedNote);
    });

    connect(mClip->getMidiData().get(), &Audio::MidiData::noteAdded, [=](const QSharedPointer<Audio::MidiNote>& note) {
        if (note->getMidiMessage().isNoteOff())
            return ;

        auto widget = new NoteVelocity(note, this);
        widget->show();

        mVelocityWidgets.push_back(widget);

        updateNote(note);
    });

    connect(mClip->getMidiData().get(), &Audio::MidiData::noteRemoved, [=](const QSharedPointer<Audio::MidiNote>& note) {
        if (note->getMidiMessage().isNoteOff())
            return ;

        for (const auto& widget : mVelocityWidgets) {
            if (widget->getCorrespondingMidiNote() == note) {
                mVelocityWidgets.remove(mVelocityWidgets.indexOf(widget));
                widget->deleteLater();
            }
        }
    });
}

void VelocityPanel::updateNote(const QSharedPointer<Audio::MidiNote> &note)
{
    for (const auto& widget : mVelocityWidgets) {
        if (widget->getCorrespondingMidiNote() == note || widget->getCorrespondingMidiNote()->getNoteOffObject() == note) {
            auto associatedNote = widget->getCorrespondingMidiNote();

            double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
                ->getProjectProperties()->getBpm();

            int x = Utils::samplesToPixels(associatedNote->getPositionInSamples(), mTimelineProperties->getPixelsPerBeatAmount(), bpm);
            int width = Utils::samplesToPixels(associatedNote->getNoteOffObject()->getPositionInSamples() - associatedNote->getPositionInSamples(), mTimelineProperties->getPixelsPerBeatAmount(), bpm);

            widget->setGeometry(x - 5 + DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH,
                                30,
                                width + 5,
                                10);
        }
    }
}

void VelocityPanel::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor(212, 212, 216)));

    double division = Utils::calculateDivision(mTimelineProperties->getPixelsPerBeatAmount(), LARGE);

    int pixelsInDivision = int(mTimelineProperties->getPixelsPerBeatAmount() * division);
    if (pixelsInDivision == 0)
        return;

    for (int i = 0; i * pixelsInDivision < width(); i++) {
        p.drawLine(i * pixelsInDivision + DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH, 0, i * pixelsInDivision + DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH, height());
    }
}

}
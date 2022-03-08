#include "midieditor.h"

namespace Graphics
{

MidiEditor::MidiEditor(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent) : QWidget(parent), mClip(clip)
{
    int totalNotes = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    setMinimumHeight(totalNotes * 20);
}

double MidiEditor::calculatePixelsPerBeatAmount() const
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()->getProjectProperties()->getBpm();
    double samplesInBeat = samplesPerMinute / bpm;
    return (double) width() / (double) mClip->getClipProperties()->getLengthInSamples() * samplesInBeat;
}

void MidiEditor::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor("#E4E4E7")));

    double division = Utils::calculateDivision(mCurrentPixelsPerBeatAmount);

    int pixelsInDivision = int(mCurrentPixelsPerBeatAmount * division);
    if (pixelsInDivision == 0)
        return;

    int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    for (int i = 0; i < nbNotesToDraw; i++) {
        p.drawLine(0, i * 20, width(), i * 20);
    }
    for (int i = 0; i * pixelsInDivision < width(); i++) {
        p.drawLine(i * pixelsInDivision, 0, i * pixelsInDivision, height());
    }
}

void MidiEditor::resizeEvent(QResizeEvent *)
{
    mCurrentPixelsPerBeatAmount = calculatePixelsPerBeatAmount();
    emit pixelsPerBeatAmountChanged(mCurrentPixelsPerBeatAmount);
    update();
}

} // Graphics
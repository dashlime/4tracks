#include "midieditor.h"

namespace Graphics
{

MidiEditor::MidiEditor(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent) : QWidget(parent), mClip(clip)
{

}

void MidiEditor::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor("#E4E4E7")));

    int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    for (int i = 0; i < nbNotesToDraw; i++) {
        p.fillRect(70, i * 20, 20, 20, mClip->currentScaleContains(i) ? QColorConstants::White : QColorConstants::Black);

        p.drawLine(0, i * 20, width(), i * 20);
    }
    p.drawLine(0, 0, 0, height());
    p.drawLine(70, 0, 70, height());
    p.drawLine(90, 0, 90, height());

    double samplesInBeat = DEFAULT_SAMPLE_RATE * 60 / mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()->getProjectProperties()->getBpm();
    double pixelsPerBeatAmount = double(width() - (70 + 20)) / (double) mClip->getClipProperties()->getLengthInSamples() * samplesInBeat;
    double division = Utils::calculateDivision(pixelsPerBeatAmount);

    int pixelsInDivision = int(pixelsPerBeatAmount * division);
    if (pixelsInDivision == 0)
        return;

    p.translate(90, 0);
    for (int i = 0; i * pixelsInDivision < width(); i++) {
        p.drawLine(i * pixelsInDivision, 0, i * pixelsInDivision, height());
    }

    p.setPen(QColorConstants::Black);

    p.translate(-90, 0);
    for (int i = 0; i < nbNotesToDraw; i++) {
        p.drawText(QRect(5, i * 20, 65, 20), Qt::AlignVCenter | Qt::AlignLeft,mClip->midiNoteName(nbNotesToDraw - i));
    }
}

} // Graphics
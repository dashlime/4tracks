#include "midieditor.h"

namespace Graphics
{

MidiEditor::MidiEditor(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent) : QWidget(parent), mClip(clip), mCurrentPixelsPerBeatAmount(calculatePixelsPerBeatAmount())
{
    int totalNotes = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    setMinimumHeight(totalNotes * 20);

    connect(this, &MidiEditor::pixelsPerBeatAmountChanged, [=](double newPixelsPerBeatAmount)
    {
        refreshMidiNotes();
    });

    connect(mClip.get(), &Audio::Clip::dataChanged, [=]()
    {
        refreshMidiNotes();
    });
}

double MidiEditor::calculatePixelsPerBeatAmount() const
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()->getProjectProperties()->getBpm();
    double samplesInBeat = samplesPerMinute / bpm;
    return (double) (width() - 1) / (double) mClip->getClipProperties()->getLengthInSamples() * samplesInBeat;
}

void MidiEditor::refreshMidiNotes()
{
    for (const auto& note : mNotes)
        note->deleteLater();

    mNotes.clear();

    for (const auto& midiNote : mClip->getMidiData()->getMidiNotes()) {
        if (midiNote->getMidiMessage().isNoteOn() && midiNote->getNoteOffObject()) {
            double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
                ->getProjectProperties()->getBpm();

            auto *note = new MidiNote(midiNote, mClip, this);
            int width = Utils::samplesToPixels(midiNote->getNoteOffObject()->getPositionInSamples() - midiNote->getPositionInSamples(), mCurrentPixelsPerBeatAmount, bpm);

            int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);

            note->setGeometry(Utils::samplesToPixels(midiNote->getPositionInSamples(), mCurrentPixelsPerBeatAmount, bpm) + 1,
                              (nbNotesToDraw - midiNote->getMidiMessage().getNoteNumber()) * 20 + 1,
                              width,
                              19);
            note->show();

            mNotes.push_back(note);
        }
    }
}

void MidiEditor::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QPen(QColor("#D4D4D8")));

    double division = Utils::calculateDivision(mCurrentPixelsPerBeatAmount, LARGE);

    double pixelsInDivision = mCurrentPixelsPerBeatAmount * division;
    if (pixelsInDivision == 0)
        return;

    int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    for (int i = 0; i < nbNotesToDraw; i++) {
        if (!mClip->currentScaleContains(nbNotesToDraw - i)) {
            p.fillRect(0, i * 20, width(), 20, QColor("#E4E4E7"));
        }

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

void MidiEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
    bool noteClicked = false;
    for (const auto& note: mNotes) {
        if (note->geometry().contains(event->pos())) {
            noteClicked = true;

            mClip->getMidiData()->removeNote(note->getMidiNote());
        }
    }

    if (!noteClicked) {
        double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
            ->getProjectProperties()->getBpm();
        juce::int64 notePositionInSamples = Utils::pixelsToSamples(event->pos().x(), mCurrentPixelsPerBeatAmount, bpm);
        notePositionInSamples =
            Utils::roundPosition(notePositionInSamples, mCurrentPixelsPerBeatAmount, bpm, true, LARGE);

        int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
        int midiNoteNumber = nbNotesToDraw - event->pos().y() / 20;

        int pixelsInDivision =
            int(Utils::calculateDivision(mCurrentPixelsPerBeatAmount, LARGE) * mCurrentPixelsPerBeatAmount);

        juce::int64 samplesInDivision = Utils::pixelsToSamples(pixelsInDivision, mCurrentPixelsPerBeatAmount, bpm);

        auto noteOff = QSharedPointer<Audio::MidiNote>::create(
            notePositionInSamples + samplesInDivision,
            juce::MidiMessage::noteOff(1, midiNoteNumber));

        auto noteOn = QSharedPointer<Audio::MidiNote>::create(
            notePositionInSamples,
            juce::MidiMessage::noteOn(1, midiNoteNumber, 1.f));

        noteOn->setNoteOffObject(noteOff);

        mClip->getMidiData()->addNote(noteOn);
        mClip->getMidiData()->addNote(noteOff);
    }
}

} // Graphics
#include "midieditor.h"

namespace Graphics
{

MidiEditor::MidiEditor(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent)
    : QWidget(parent), mClip(clip), mCurrentPixelsPerBeatAmount(calculatePixelsPerBeatAmount())
{
    int totalNotes = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    setMinimumHeight(totalNotes * 20);

    setupMidiNotes();
    setMouseTracking(true);

    connect(this, &MidiEditor::pixelsPerBeatAmountChanged, [=](double newPixelsPerBeatAmount)
    {
        refreshMidiNotes();
    });

    connect(mClip->getMidiData().get(),
            &Audio::MidiData::noteAdded,
            [=](const QSharedPointer<Audio::MidiNote> &noteAdded)
            {
                if (noteAdded->getMidiMessage().isNoteOff())
                    return;

                auto note = new MidiNote(noteAdded, mClip, this);

                note->show();
                note->setMouseTracking(true);

                mNotes.push_back(note);
                refreshMidiNotes();
            });

    connect(mClip->getMidiData().get(),
            &Audio::MidiData::noteRemoved,
            [=](const QSharedPointer<Audio::MidiNote> &noteRemoved)
            {
                int i = 0;
                for (const auto &note: mNotes) {
                    if (note->getMidiNote() == noteRemoved) {
                        note->deleteLater();
                        mNotes.remove(i);
                    }
                    i++;
                }

                mCurrentSelection.setSelectionType(SelectionManager::NoSelection);

                refreshMidiNotes();
            });
}

double MidiEditor::calculatePixelsPerBeatAmount() const
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    double bpm =
        mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()->getProjectProperties()
            ->getBpm();
    double samplesInBeat = samplesPerMinute / bpm;
    return (double) (width() - 1) / (double) mClip->getClipProperties()->getLengthInSamples() * samplesInBeat;
}

void MidiEditor::updateNoteGeometry(const QPointer<MidiNote>& note)
{
    double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
        ->getProjectProperties()->getBpm();

    int width = Utils::samplesToPixels(note->getMidiNote()->getNoteOffObject()->getPositionInSamples()
                                           - note->getMidiNote()->getPositionInSamples(),
                                       mCurrentPixelsPerBeatAmount,
                                       bpm);
    int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);

    note->setGeometry(
        Utils::samplesToPixels(note->getMidiNote()->getPositionInSamples(), mCurrentPixelsPerBeatAmount, bpm)
            + 1,
        (nbNotesToDraw - note->getMidiNote()->getMidiMessage().getNoteNumber()) * 20 + 1,
        width,
        19);
}

void MidiEditor::refreshMidiNotes()
{
    for (const auto &midiNote: mNotes) {
        updateNoteGeometry(midiNote);
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
    for (const auto &note: mNotes) {
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
            juce::MidiMessage::noteOff(DEFAULT_MIDI_CHANNEL, midiNoteNumber));

        auto noteOn = QSharedPointer<Audio::MidiNote>::create(
            notePositionInSamples,
            juce::MidiMessage::noteOn(DEFAULT_MIDI_CHANNEL, midiNoteNumber, 1.f));

        noteOn->setNoteOffObject(noteOff);

        mClip->getMidiData()->addNote(noteOn);
        mClip->getMidiData()->addNote(noteOff);
    }
}

void MidiEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons().testFlag(Qt::NoButton)) {
        setCursor(Qt::ArrowCursor);
        for (const auto &note: mNotes) {
            if (note->geometry().contains(event->pos())) {
                auto pos = event->pos();
                pos -= note->pos();

                if (pos.x() >= note->width() - 3 || pos.x() < 3) {
                    setCursor(Qt::SizeHorCursor);
                    setCursor(Qt::SizeHorCursor);
                }
            }
        }
    }
    else if (event->buttons().testFlag(Qt::LeftButton) && mMovingNoteReference != nullptr) {
        if (mExtendingByTheRight) {
            handleExtendingByTheRight(event);
        } else if (mExtendingByTheLeft) {
            handleExtendingByTheLeft(event);
        }
    }
}

void MidiEditor::mousePressEvent(QMouseEvent *event)
{
    mClickedPos = event->pos();

    for (const auto &note: mNotes) {
        if (note->geometry().contains(event->pos())) {
            auto pos = event->pos();
            pos -= note->pos();

            if (pos.x() >= note->width() - 3) {
                mExtendingByTheRight = true;
                mExtendingByTheLeft = false;

                mMovingNoteReference = note;
                if (!mCurrentSelection.getSelectedObjects().contains(note))
                    mCurrentSelection.objectSelected(note, event);
            }
            else if (pos.x() < 3) {
                mExtendingByTheRight = false;
                mExtendingByTheLeft = true;

                mMovingNoteReference = note;
                if (!mCurrentSelection.getSelectedObjects().contains(note))
                    mCurrentSelection.objectSelected(note, event);
            }
            else {
                mExtendingByTheRight = false;
                mExtendingByTheLeft = false;

                mCurrentSelection.objectSelected(note, event);
            }
        }
    }
}

void MidiEditor::setupMidiNotes()
{
    mCurrentSelection.setSelectionType(SelectionManager::NoSelection);

    for (const auto &note: mNotes)
        note->deleteLater();

    mNotes.clear();

    for (const auto &midiNote: mClip->getMidiData()->getMidiNotes()) {
        if (midiNote->getMidiMessage().isNoteOn() && midiNote->getNoteOffObject()) {
            auto *note = new MidiNote(midiNote, mClip, this);
            note->show();

            note->setMouseTracking(true);

            mNotes.push_back(note);
        }
    }
}
void MidiEditor::mouseReleaseEvent(QMouseEvent *event)
{
    mExtendingByTheRight = false;
    mExtendingByTheLeft = true;

    mMovingNoteReference = nullptr;
}

void MidiEditor::handleExtendingByTheLeft(QMouseEvent *event)
{
    auto roundedPosition = calculateCurrentRoundedPositionInSamples(event);

    juce::int64 difference = roundedPosition - mMovingNoteReference->getMidiNote()->getPositionInSamples();

    if (mMovingNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples() - roundedPosition <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
        roundedPosition = mMovingNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples() - MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES;

    mMovingNoteReference->getMidiNote()->setPositionInSamples(roundedPosition);

    updateNoteGeometry(mMovingNoteReference);

    for (const auto &object: mCurrentSelection.getSelectedObjects()) {
        if (object->getType() == SelectionManager::SelectableObject::MidiNote && object != mMovingNoteReference) {
            auto note = (MidiNote *) object.get();
            auto startPos = note->getMidiNote()->getPositionInSamples();
            auto endPos = note->getMidiNote()->getNoteOffObject()->getPositionInSamples();

            note->getMidiNote()->setPositionInSamples(startPos + difference);

            if (endPos - (startPos + difference) <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
                note->getMidiNote()->setPositionInSamples(endPos - MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES);
            updateNoteGeometry(note);
        }
    }
}

void MidiEditor::handleExtendingByTheRight(QMouseEvent *event)
{
    auto roundedPosition = calculateCurrentRoundedPositionInSamples(event);

    juce::int64 difference = roundedPosition - mMovingNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples();

    if (roundedPosition - mMovingNoteReference->getMidiNote()->getPositionInSamples() <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
        roundedPosition = mMovingNoteReference->getMidiNote()->getPositionInSamples() + MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES;

    mMovingNoteReference->getMidiNote()->getNoteOffObject()->setPositionInSamples(roundedPosition);

    updateNoteGeometry(mMovingNoteReference);

    for (const auto &object: mCurrentSelection.getSelectedObjects()) {
        if (object->getType() == SelectionManager::SelectableObject::MidiNote && object != mMovingNoteReference) {
            auto note = (MidiNote *) object.get();
            auto startPos = note->getMidiNote()->getPositionInSamples();
            auto endPos = note->getMidiNote()->getNoteOffObject()->getPositionInSamples();

            note->getMidiNote()->getNoteOffObject()->setPositionInSamples(endPos + difference);

            if (endPos + difference - startPos <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
                note->getMidiNote()->getNoteOffObject()->setPositionInSamples(startPos + MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES);
            updateNoteGeometry(note);
        }
    }
}
juce::int64 MidiEditor::calculateCurrentRoundedPositionInSamples(QMouseEvent *event) const
{
    double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
        ->getProjectProperties()->getBpm();

    juce::int64 currentPosInSamples =
        Utils::pixelsToSamples(event->pos().x(), mCurrentPixelsPerBeatAmount, bpm);
    juce::int64 roundedPosition =
        Utils::roundPosition(currentPosInSamples, mCurrentPixelsPerBeatAmount, bpm, false, LARGE);

    return roundedPosition;
}

} // Graphics
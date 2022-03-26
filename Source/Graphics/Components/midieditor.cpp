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

    connect(mClip->getMidiData().get(), &Audio::MidiData::noteAdded, [=](const QSharedPointer<Audio::MidiNote> &noteAdded)
    {
        if (noteAdded->getMidiMessage().isNoteOff())
            return;

        auto note = new MidiNote(noteAdded, mClip, this);

        note->show();
        note->setMouseTracking(true);

        mNotes.push_back(note);
        refreshMidiNotes();
    });

    connect(mClip->getMidiData().get(), &Audio::MidiData::noteRemoved, [=](const QSharedPointer<Audio::MidiNote> &noteRemoved)
    {
        int i = 0;
        for (const auto& note : mNotes) {
            if (note->getMidiNote() == noteRemoved) {
                note->deleteLater();
                mNotes.remove(i);
            }
            i++;
        }

        mCurrentSelection.setSelectionType(Selection::NoSelection);

        refreshMidiNotes();
    });

    setupMidiNotes();
    setMouseTracking(true);
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
    for (const auto& midiNote : mNotes) {
        double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
            ->getProjectProperties()->getBpm();

        int width = Utils::samplesToPixels(midiNote->getMidiNote()->getNoteOffObject()->getPositionInSamples() - midiNote->getMidiNote()->getPositionInSamples(), mCurrentPixelsPerBeatAmount, bpm);
        int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);

        midiNote->setGeometry(Utils::samplesToPixels(midiNote->getMidiNote()->getPositionInSamples(), mCurrentPixelsPerBeatAmount, bpm) + 1,
                          (nbNotesToDraw - midiNote->getMidiNote()->getMidiMessage().getNoteNumber()) * 20 + 1,
                          width,
                          19);
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
        for (const auto& note: mNotes) {
            if (note->geometry().contains(event->pos())) {
                auto pos = event->pos();
                pos -= note->pos();

                if (pos.x() >= note->width() - 3 || pos.x() < 3) {
                    setCursor(Qt::SizeHorCursor);
                    setCursor(Qt::SizeHorCursor);
                }
            }
        }
    } else if (event->buttons().testFlag(Qt::LeftButton)) {
        double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
            ->getProjectProperties()->getBpm();

        if (mExtendingByTheRight) {
            for (const auto& object: mCurrentSelection.getSelectedObjects()) {
                if (object->getType() == Selection::SelectableObject::MidiNote) {
                    auto note = (MidiNote *) object.get();

                    juce::int64 currentPosInSamples = Utils::pixelsToSamples(event->pos().x(), mCurrentPixelsPerBeatAmount, bpm);
                    juce::int64 roundedPosition = Utils::roundPosition(currentPosInSamples, mCurrentPixelsPerBeatAmount, bpm, false, LARGE);

                    note->setGeometry(note->x(), note->y(), Utils::samplesToPixels(roundedPosition, mCurrentPixelsPerBeatAmount, bpm) - note->x(), note->height());
                }
            }
        }
    }
}

void MidiEditor::mousePressEvent(QMouseEvent *event)
{
    mClickedPos = event->pos();

    for (const auto& note: mNotes) {
        if (note->geometry().contains(event->pos())) {
            auto pos = event->pos();
            pos -= note->pos();

            if (pos.x() >= note->width() - 3) {
                mExtendingByTheRight = true;
                mExtendingByTheLeft = false;
            } else if (pos.x() < 3) {
                mExtendingByTheRight = false;
                mExtendingByTheLeft = true;
            } else {
                mExtendingByTheRight = false;
                mExtendingByTheLeft = false;
            }

            mCurrentSelection.objectSelected(note, event);
        }
    }
}

void MidiEditor::setupMidiNotes()
{
    mCurrentSelection.setSelectionType(Selection::NoSelection);

    for (const auto& note : mNotes)
        note->deleteLater();

    mNotes.clear();

    for (const auto& midiNote : mClip->getMidiData()->getMidiNotes()) {
        if (midiNote->getMidiMessage().isNoteOn() && midiNote->getNoteOffObject()) {
            auto *note = new MidiNote(midiNote, mClip, this);
            note->show();

            note->setMouseTracking(true);

            mNotes.push_back(note);
        }
    }
}

} // Graphics
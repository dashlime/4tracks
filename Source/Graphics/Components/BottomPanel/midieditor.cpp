#include "midieditor.h"

namespace Graphics
{

MidiEditor::MidiEditor(const QSharedPointer<TimelineProperties> &timelineProperties, const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent)
    : QWidget(parent), mClip(clip), mSelectionOverlay(false, this), mTimelineProperties(timelineProperties)
{
    updatePixelsPerBeatAmount();

    int totalNotes = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    setMinimumHeight(totalNotes * DEFAULT_NOTE_HEIGHT);

    setupMidiNotes();
    setMouseTracking(true);

    connect(mTimelineProperties.get(), &TimelineProperties::pixelsPerBeatAmountChanged, [=]()
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
                mTimelineProperties->getCurrentSelection()->setSelectionType(SelectionManager::NoSelection);

                int i = 0;
                for (const auto &note: mNotes) {
                    if (note->getMidiNote() == noteRemoved) {
                        note->deleteLater();
                        mNotes.remove(i);
                    }
                    i++;
                }

                refreshMidiNotes();
            });

    connect(mClip->getMidiData().get(), &Audio::MidiData::noteDataChanged, [=](const QSharedPointer<Audio::MidiNote> &noteChanged)
    {
        updateNoteGeometry(getGraphicsNoteForAudioNote(noteChanged));
    });

    mSelectionOverlay.setParent(this);
}

void MidiEditor::updatePixelsPerBeatAmount()
{
    double samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    double bpm =
        mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()->getProjectProperties()
            ->getBpm();
    double samplesInBeat = samplesPerMinute / bpm;
    mTimelineProperties->setBasePixelsPerBeatAmount((double) (width() - 1) / (double) mClip->getClipProperties()->getLengthInSamples() * samplesInBeat);
}

QPointer<MidiNote> MidiEditor::getGraphicsNoteForAudioNote(const QSharedPointer<Audio::MidiNote>& note) const
{
    for (auto uiNote: mNotes)
        if (uiNote->getMidiNote() == note || uiNote->getMidiNote()->getNoteOffObject() == note)
            return uiNote;

    return {};
}

void MidiEditor::updateNoteGeometry(const QPointer<MidiNote>& note)
{
    double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
        ->getProjectProperties()->getBpm();

    int width = Utils::samplesToPixels(note->getMidiNote()->getNoteOffObject()->getPositionInSamples()
                                           - note->getMidiNote()->getPositionInSamples(),
                                       mTimelineProperties->getPixelsPerBeatAmount(),
                                       bpm);
    int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);

    note->setGeometry(
        Utils::samplesToPixels(note->getMidiNote()->getPositionInSamples(), mTimelineProperties->getPixelsPerBeatAmount(), bpm)
            + 1,
        (nbNotesToDraw - note->getMidiNote()->getMidiMessage().getNoteNumber()) * DEFAULT_NOTE_HEIGHT + 1,
        width,
        19);
}

void MidiEditor::refreshMidiNotes()
{
    for (const auto &midiNote: mNotes) {
        updateNoteGeometry(midiNote);
    }
}

void MidiEditor::updateSelectionOverlay()
{
    mSelectionOverlay.areaChanged(mCurrentSelectionArea);
    mSelectionOverlay.raise();
}

void MidiEditor::updateSelectedNotes()
{
    QVector<QPointer<SelectableObject>> objects;
    for (const auto& note: mNotes)
        if (mCurrentSelectionArea.contains(note->geometry().topLeft()) ||
            mCurrentSelectionArea.contains(note->geometry().topRight()) ||
            mCurrentSelectionArea.contains(note->geometry().bottomLeft()) ||
            mCurrentSelectionArea.contains(note->geometry().bottomRight()))
            objects.push_back(note.get());

    mTimelineProperties->getCurrentSelection()->setSelectedObjects(objects);
}

void MidiEditor::paintEvent(QPaintEvent *event)
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

    int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    for (int i = 0; i < nbNotesToDraw; i++) {
        if (!mClip->currentScaleContains(nbNotesToDraw - i)) {
            p.fillRect(0, i * DEFAULT_NOTE_HEIGHT, width(), DEFAULT_NOTE_HEIGHT, QColor(228, 228, 231));
        }

        p.drawLine(0, i * DEFAULT_NOTE_HEIGHT, width(), i * DEFAULT_NOTE_HEIGHT);
    }

    for (int i = 0; i * pixelsInDivision < width(); i++) {
        p.drawLine(i * pixelsInDivision, 0, i * pixelsInDivision, height());
    }
}

void MidiEditor::resizeEvent(QResizeEvent *)
{
    updatePixelsPerBeatAmount();

    mSelectionOverlay.setGeometry(0, 0, width(), height());
    mSelectionOverlay.raise();
    updateSelectionOverlay();

    update();
}

void MidiEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
    bool noteClicked = false;
    auto note = getNoteUnderCursor(event);
    if (!note.isNull()) {
        noteClicked = true;
        mClip->getMidiData()->removeNote(note->getMidiNote());
    }

    if (!noteClicked) {
        juce::int64 notePositionInSamples = calculateCurrentRoundedPositionInSamples(event, true);
        auto samplesInDivision = calculateSamplesInDivisionAmount();

        int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
        int midiNoteNumber = nbNotesToDraw - event->pos().y() / DEFAULT_NOTE_HEIGHT;

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
        auto note = getNoteUnderCursor(event);

        if (!note.isNull()) {
            auto pos = event->pos();
            pos -= note->pos();

            if (pos.x() >= note->width() - 3 || pos.x() < 3) {
                setCursor(Qt::SizeHorCursor);
                setCursor(Qt::SizeHorCursor);
            }
        }
    } else if (event->buttons().testFlag(Qt::LeftButton) && mActionNoteReference != nullptr) {
        if (mExtendingByTheRight) {
            handleExtendingByTheRight(event);
        } else if (mExtendingByTheLeft) {
            handleExtendingByTheLeft(event);
        } else if (mMoving) {
            handleMovingEvent(event);
        }
    } else if (event->buttons().testFlag(Qt::LeftButton)) {
        mCurrentSelectionArea.setX(mClickedPos.x());
        mCurrentSelectionArea.setY(mClickedPos.y());
        mCurrentSelectionArea.setWidth(event->pos().x() - mClickedPos.x());
        mCurrentSelectionArea.setHeight(event->pos().y() - mClickedPos.y());

        updateSelectionOverlay();
        updateSelectedNotes();
    }
}

void MidiEditor::mousePressEvent(QMouseEvent *event)
{
    mMoving = false;
    mClickedPos = event->pos();

    bool rightClick = event->buttons().testFlag(Qt::RightButton);

    auto note = getNoteUnderCursor(event);
    if (note.isNull()) {
        mTimelineProperties->getCurrentSelection()->setSelectionType(SelectionManager::NoSelection);
    } else {
        mTimelineProperties->getCurrentSelection()->handleMousePressEvent(note.get(), event);
        if (rightClick) {
            QMenu menu(this);

            auto *deleteAction = new QAction("Delete", this);
            menu.addAction(deleteAction);

            connect(deleteAction, &QAction::triggered, [this]()
            {
                for (const auto &obj: mTimelineProperties->getCurrentSelection()->getSelectedObjects()) {
                    auto note = (MidiNote *) obj.get();
                    mClip->getMidiData()->removeNote(note->getMidiNote());
                }
            });
            menu.exec(event->globalPosition().toPoint());
        }
        else {
            auto pos = event->pos();
            pos -= note->pos();
            mActionNoteReference = note;

            if (pos.x() >= note->width() - 3) {
                mExtendingByTheRight = true;
            }
            else if (pos.x() < 3) {
                mExtendingByTheLeft = true;
            }
            else {
                mMoving = true;
            }
        }
    }
}

void MidiEditor::mouseReleaseEvent(QMouseEvent *event)
{
    mExtendingByTheRight = false;
    mExtendingByTheLeft = false;
    mMoving = false;

    mActionNoteReference = nullptr;
    mMovingDifferenceAmount = QPoint(0, 0);

    mTimelineProperties->getCurrentSelection()->handleMouseReleaseEvent();

    mCurrentSelectionArea = {};
    updateSelectionOverlay();
}

void MidiEditor::setupMidiNotes()
{
    mTimelineProperties->getCurrentSelection()->setSelectionType(SelectionManager::NoSelection);

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

void MidiEditor::handleExtendingByTheLeft(QMouseEvent *event)
{
    auto roundedPosition = calculateCurrentRoundedPositionInSamples(event);

    juce::int64 difference = roundedPosition - mActionNoteReference->getMidiNote()->getPositionInSamples();

    if (mActionNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples() - roundedPosition <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
        roundedPosition = mActionNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples() - MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES;

    mActionNoteReference->getMidiNote()->setPositionInSamples(roundedPosition);

    for (const auto &object: mTimelineProperties->getCurrentSelection()->getSelectedObjects()) {
        if (object->getType() == SelectableObject::MidiNote && object != mActionNoteReference) {
            auto note = (MidiNote *) object.get();
            auto startPos = note->getMidiNote()->getPositionInSamples();
            auto endPos = note->getMidiNote()->getNoteOffObject()->getPositionInSamples();

            note->getMidiNote()->setPositionInSamples(startPos + difference);

            if (endPos - (startPos + difference) <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
                note->getMidiNote()->setPositionInSamples(endPos - MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES);
        }
    }

    mTimelineProperties->getCurrentSelection()->clearPendingEvent();
}

void MidiEditor::handleExtendingByTheRight(QMouseEvent *event)
{
    auto roundedPosition = calculateCurrentRoundedPositionInSamples(event);

    juce::int64 difference = roundedPosition - mActionNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples();

    if (roundedPosition - mActionNoteReference->getMidiNote()->getPositionInSamples() <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
        roundedPosition = mActionNoteReference->getMidiNote()->getPositionInSamples() + MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES;

    mActionNoteReference->getMidiNote()->getNoteOffObject()->setPositionInSamples(roundedPosition);

    for (const auto &object: mTimelineProperties->getCurrentSelection()->getSelectedObjects()) {
        if (object->getType() == SelectableObject::MidiNote && object != mActionNoteReference) {
            auto note = (MidiNote *) object.get();
            auto startPos = note->getMidiNote()->getPositionInSamples();
            auto endPos = note->getMidiNote()->getNoteOffObject()->getPositionInSamples();

            note->getMidiNote()->getNoteOffObject()->setPositionInSamples(endPos + difference);

            if (endPos + difference - startPos <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
                note->getMidiNote()->getNoteOffObject()->setPositionInSamples(startPos + MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES);
        }
    }

    mTimelineProperties->getCurrentSelection()->clearPendingEvent();
}

void MidiEditor::handleMovingEvent(QMouseEvent *event)
{
    int nbNotesDrawn = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);

    auto newDiffX = calculatePositionInSamples(event->pos().x() - mClickedPos.x() - mMovingDifferenceAmount.x(), true);
    int newDiffY = (int) round(double(event->pos().y() - mClickedPos.y() - mMovingDifferenceAmount.y()) / DEFAULT_NOTE_HEIGHT);

    auto startPos = mActionNoteReference->getMidiNote()->getPositionInSamples();
    auto endPos = mActionNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples();
    mActionNoteReference->getMidiNote()->setPositionInSamples(startPos + newDiffX);
    mActionNoteReference->getMidiNote()->getNoteOffObject()->setPositionInSamples(endPos + newDiffX);

    auto noteNumber = mActionNoteReference->getMidiNote()->getMidiMessage().getNoteNumber() - newDiffY;
    if (noteNumber > nbNotesDrawn)
        noteNumber = nbNotesDrawn;

    if (noteNumber < 1)
        noteNumber = 1;

    auto message = mActionNoteReference->getMidiNote()->getMidiMessage();
    message.setNoteNumber(noteNumber);
    mActionNoteReference->getMidiNote()->setMidiMessage(message);
    message = mActionNoteReference->getMidiNote()->getNoteOffObject()->getMidiMessage();
    message.setNoteNumber(noteNumber);
    mActionNoteReference->getMidiNote()->getNoteOffObject()->setMidiMessage(message);

    for (const auto &object: mTimelineProperties->getCurrentSelection()->getSelectedObjects()) {
        if (object->getType() == SelectableObject::MidiNote && object != mActionNoteReference) {
            auto note = (MidiNote *) object.get();
            startPos = note->getMidiNote()->getPositionInSamples();
            endPos = note->getMidiNote()->getNoteOffObject()->getPositionInSamples();

            note->getMidiNote()->setPositionInSamples(startPos + newDiffX);
            note->getMidiNote()->getNoteOffObject()->setPositionInSamples(endPos + newDiffX);

            noteNumber = note->getMidiNote()->getMidiMessage().getNoteNumber() - newDiffY;
            if (noteNumber > nbNotesDrawn)
                noteNumber = nbNotesDrawn;

            if (noteNumber < 1)
                noteNumber = 1;

            message = note->getMidiNote()->getMidiMessage();
            message.setNoteNumber(noteNumber);
            note->getMidiNote()->setMidiMessage(message);
            message = note->getMidiNote()->getNoteOffObject()->getMidiMessage();
            message.setNoteNumber(noteNumber);
            note->getMidiNote()->getNoteOffObject()->setMidiMessage(message);
        }
    }
    double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
        ->getProjectProperties()->getBpm();

    mMovingDifferenceAmount.setX(mMovingDifferenceAmount.x() + Utils::samplesToPixels(newDiffX, mTimelineProperties->getPixelsPerBeatAmount(), bpm));
    mMovingDifferenceAmount.setY(mMovingDifferenceAmount.y() + newDiffY * DEFAULT_NOTE_HEIGHT);

    mTimelineProperties->getCurrentSelection()->clearPendingEvent();
}

juce::int64 MidiEditor::calculateCurrentRoundedPositionInSamples(QMouseEvent *event, bool forceRoundingToPriorPosition) const
{
    return calculatePositionInSamples(event->pos().x(), true, forceRoundingToPriorPosition);
}

juce::int64 MidiEditor::calculatePositionInSamples(int x, bool rounded, bool forceRoundingToPriorPosition) const
{
    double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
        ->getProjectProperties()->getBpm();

    juce::int64 posInSamples =
        Utils::pixelsToSamples(x, mTimelineProperties->getPixelsPerBeatAmount(), bpm);

    if (rounded) {
        juce::int64 roundedPosition =
            Utils::roundPosition(posInSamples, mTimelineProperties->getPixelsPerBeatAmount(), bpm, forceRoundingToPriorPosition, LARGE);

        return roundedPosition;
    }
    return posInSamples;
}

QPointer<MidiNote> MidiEditor::getNoteUnderCursor(QMouseEvent *event) const
{
    for (const auto &note: mNotes) {
        if (note->geometry().contains(event->pos())) {
            return note;
        }
    }
    return {};
}

juce::int64 MidiEditor::calculateSamplesInDivisionAmount() const
{
    double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
        ->getProjectProperties()->getBpm();

    int pixelsInDivision =
        int(Utils::calculateDivision(mTimelineProperties->getPixelsPerBeatAmount(), LARGE) * mTimelineProperties->getPixelsPerBeatAmount());

    juce::int64 samplesInDivision = Utils::pixelsToSamples(pixelsInDivision, mTimelineProperties->getPixelsPerBeatAmount(), bpm);
    return samplesInDivision;
}

} // Graphics
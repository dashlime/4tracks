#include "midieditor.h"

namespace Graphics
{

MidiEditor::MidiEditor(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent)
    : QWidget(parent), mClip(clip), mCurrentPixelsPerBeatAmount(calculatePixelsPerBeatAmount())
{
    int totalNotes = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);
    setMinimumHeight(totalNotes * DEFAULT_NOTE_HEIGHT);

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
                mCurrentSelection.setSelectionType(SelectionManager::NoSelection);

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

    connect(&mCurrentSelection, &SelectionManager::selectionChanged, [=]()
    {
        updateSelectionOverlay();
    });

    mSelectionOverlay.setParent(this);
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
                                       mCurrentPixelsPerBeatAmount,
                                       bpm);
    int nbNotesToDraw = NOTES_IN_OCTAVE * (END_MIDI_OCTAVE - START_MIDI_OCTAVE);

    note->setGeometry(
        Utils::samplesToPixels(note->getMidiNote()->getPositionInSamples(), mCurrentPixelsPerBeatAmount, bpm)
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
    SelectionManager::SelectionArea area = mCurrentSelection.getSelectedArea();

    auto bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()->getProjectProperties()->getBpm();
    int x = Utils::samplesToPixels(area.startSample, mCurrentPixelsPerBeatAmount, bpm);
    int w = Utils::samplesToPixels(area.nbSamples, mCurrentPixelsPerBeatAmount, bpm);
    int y, h;

    if (area.nbTracks > 0) {
        y = area.startTrackIndex * DEFAULT_NOTE_HEIGHT;
        h = area.nbTracks * DEFAULT_NOTE_HEIGHT;
    }
    else {
        y = (area.startTrackIndex + 1) * DEFAULT_NOTE_HEIGHT;
        h = area.nbTracks * DEFAULT_NOTE_HEIGHT;
    }

    mSelectionOverlay.areaChanged(QRect(x, y, w, h));
    mSelectionOverlay.raise();
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
            p.fillRect(0, i * DEFAULT_NOTE_HEIGHT, width(), DEFAULT_NOTE_HEIGHT, QColor("#E4E4E7"));
        }

        p.drawLine(0, i * DEFAULT_NOTE_HEIGHT, width(), i * DEFAULT_NOTE_HEIGHT);
    }

    for (int i = 0; i * pixelsInDivision < width(); i++) {
        p.drawLine(i * pixelsInDivision, 0, i * pixelsInDivision, height());
    }
}

void MidiEditor::resizeEvent(QResizeEvent *)
{
    mCurrentPixelsPerBeatAmount = calculatePixelsPerBeatAmount();
    emit pixelsPerBeatAmountChanged(mCurrentPixelsPerBeatAmount);

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
        } else if (mMoving){
            handleMovingEvent(event);
        }
    } else if (event->buttons().testFlag(Qt::LeftButton)) {
        int trackIndex = event->pos().y() / DEFAULT_NOTE_HEIGHT;
        juce::int64 samples = calculatePositionInSamples(event->pos().x(), true);

        SelectionManager::SelectionArea area = mCurrentSelection.getSelectedArea();
        if (mCurrentSelection.getSelectionType() != SelectionManager::AreaSelected) {
            mCurrentSelection.setSelectionType(SelectionManager::AreaSelected);
            area.startTrackIndex = trackIndex;
            area.startSample = samples;
        }

        if (trackIndex >= area.startTrackIndex)
            area.nbTracks = trackIndex - area.startTrackIndex + 1;
        else
            area.nbTracks = trackIndex - area.startTrackIndex - 1;

        area.nbSamples = samples - area.startSample;

        mCurrentSelection.setSelectedArea(area);
    }
}

void MidiEditor::mousePressEvent(QMouseEvent *event)
{
    mMoving = false;
    mClickedPos = event->pos();

    bool rightClick = event->buttons().testFlag(Qt::RightButton);

    bool selectionAreaClicked = mSelectionOverlay.areaContains(event->pos());
    if (mCurrentSelection.getSelectionType() != SelectionManager::AreaSelected)
        selectionAreaClicked = false;

    auto note = getNoteUnderCursor(event);
    if (note.isNull() && !(selectionAreaClicked && rightClick)) {
        mCurrentSelection.setSelectionType(SelectionManager::NoSelection);

        return;
    }

    if (selectionAreaClicked) {
        if (rightClick) {
            QMenu menu(this);

            auto *deleteAction = new QAction("Delete", this);
            menu.addAction(deleteAction);

            connect(deleteAction, &QAction::triggered, [=](){
                mClip->removeArea(mCurrentSelection.getSelectedArea().startTrackIndex, mCurrentSelection.getSelectedArea().nbTracks,
                                  mCurrentSelection.getSelectedArea().startSample, mCurrentSelection.getSelectedArea().nbSamples);
            });
            menu.exec(event->globalPosition().toPoint());
        }
    } else {
        mCurrentSelection.handleMousePressEvent(note, event);
        if (rightClick) {
            QMenu menu(this);

            auto *deleteAction = new QAction("Delete", this);
            menu.addAction(deleteAction);

            connect(deleteAction, &QAction::triggered, [this](){
                for (const auto& obj: mCurrentSelection.getSelectedObjects()) {
                    auto note = (MidiNote *) obj.get();
                    mClip->getMidiData()->removeNote(note->getMidiNote());
                }
            });
            menu.exec(event->globalPosition().toPoint());
        } else {
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
    mExtendingByTheLeft = false;
    mMoving = false;

    mActionNoteReference = nullptr;
    mMovingDifferenceAmount = QPoint(0, 0);

    mCurrentSelection.handleMouseReleaseEvent();
}

void MidiEditor::handleExtendingByTheLeft(QMouseEvent *event)
{
    auto roundedPosition = calculateCurrentRoundedPositionInSamples(event);

    juce::int64 difference = roundedPosition - mActionNoteReference->getMidiNote()->getPositionInSamples();

    if (mActionNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples() - roundedPosition <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
        roundedPosition = mActionNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples() - MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES;

    mActionNoteReference->getMidiNote()->setPositionInSamples(roundedPosition);

    updateNoteGeometry(mActionNoteReference);

    for (const auto &object: mCurrentSelection.getSelectedObjects()) {
        if (object->getType() == SelectionManager::SelectableObject::MidiNote && object != mActionNoteReference) {
            auto note = (MidiNote *) object.get();
            auto startPos = note->getMidiNote()->getPositionInSamples();
            auto endPos = note->getMidiNote()->getNoteOffObject()->getPositionInSamples();

            note->getMidiNote()->setPositionInSamples(startPos + difference);

            if (endPos - (startPos + difference) <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
                note->getMidiNote()->setPositionInSamples(endPos - MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES);
            updateNoteGeometry(note);
        }
    }

    mCurrentSelection.clearPendingEvent();
}

void MidiEditor::handleExtendingByTheRight(QMouseEvent *event)
{
    auto roundedPosition = calculateCurrentRoundedPositionInSamples(event);

    juce::int64 difference = roundedPosition - mActionNoteReference->getMidiNote()->getNoteOffObject()->getPositionInSamples();

    if (roundedPosition - mActionNoteReference->getMidiNote()->getPositionInSamples() <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
        roundedPosition = mActionNoteReference->getMidiNote()->getPositionInSamples() + MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES;

    mActionNoteReference->getMidiNote()->getNoteOffObject()->setPositionInSamples(roundedPosition);

    updateNoteGeometry(mActionNoteReference);

    for (const auto &object: mCurrentSelection.getSelectedObjects()) {
        if (object->getType() == SelectionManager::SelectableObject::MidiNote && object != mActionNoteReference) {
            auto note = (MidiNote *) object.get();
            auto startPos = note->getMidiNote()->getPositionInSamples();
            auto endPos = note->getMidiNote()->getNoteOffObject()->getPositionInSamples();

            note->getMidiNote()->getNoteOffObject()->setPositionInSamples(endPos + difference);

            if (endPos + difference - startPos <= MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES)
                note->getMidiNote()->getNoteOffObject()->setPositionInSamples(startPos + MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES);
            updateNoteGeometry(note);
        }
    }

    mCurrentSelection.clearPendingEvent();
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

    updateNoteGeometry(mActionNoteReference);

    for (const auto &object: mCurrentSelection.getSelectedObjects()) {
        if (object->getType() == SelectionManager::SelectableObject::MidiNote && object != mActionNoteReference) {
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

            updateNoteGeometry(note);
        }
    }
    double bpm = mClip->getClipProperties()->getParentTrack()->getTrackProperties()->getParentProject()
        ->getProjectProperties()->getBpm();

    mMovingDifferenceAmount.setX(mMovingDifferenceAmount.x() + Utils::samplesToPixels(newDiffX, mCurrentPixelsPerBeatAmount, bpm));
    mMovingDifferenceAmount.setY(mMovingDifferenceAmount.y() + newDiffY * DEFAULT_NOTE_HEIGHT);

    mCurrentSelection.clearPendingEvent();
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
        Utils::pixelsToSamples(x, mCurrentPixelsPerBeatAmount, bpm);

    if (rounded) {
        juce::int64 roundedPosition =
            Utils::roundPosition(posInSamples, mCurrentPixelsPerBeatAmount, bpm, forceRoundingToPriorPosition, LARGE);

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
        int(Utils::calculateDivision(mCurrentPixelsPerBeatAmount, LARGE) * mCurrentPixelsPerBeatAmount);

    juce::int64 samplesInDivision = Utils::pixelsToSamples(pixelsInDivision, mCurrentPixelsPerBeatAmount, bpm);
    return samplesInDivision;
}

} // Graphics
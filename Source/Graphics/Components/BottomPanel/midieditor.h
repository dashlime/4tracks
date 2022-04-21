#ifndef MIDIEDITOR_H
#define MIDIEDITOR_H

#include <QtWidgets>

#include "Audio/Project/project.h"
#include "Constants.h"
#include "Utils/utils.h"
#include "Audio/Clip/midiclip.h"
#include "Graphics/Components/Timeline/divisionsmarker.h"
#include "Audio/Resources/mididata.h"
#include "Audio/Resources/midinote.h"
#include "midinote.h"
#include "Graphics/Managers/selectionmanager.h"
#include "Graphics/Components/Timeline/timelineproperties.h"

namespace Graphics
{

class MidiEditor: public QWidget
{
Q_OBJECT
public:
    explicit MidiEditor(const QSharedPointer<TimelineProperties> &timelineProperties, const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent = nullptr);

    void updatePixelsPerBeatAmount();

    [[nodiscard]] QPointer<MidiNote> getGraphicsNoteForAudioNote(const QSharedPointer<Audio::MidiNote>& note) const;

    void updateNoteGeometry(const QPointer<MidiNote>& note);
    void refreshMidiNotes();
    void updateSelectionOverlay();

    void updateSelectedNotes();

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void setupMidiNotes();
    void handleExtendingByTheLeft(QMouseEvent *event);
    void handleExtendingByTheRight(QMouseEvent *event);
    void handleMovingEvent(QMouseEvent *event);

    [[nodiscard]] juce::int64 calculateCurrentRoundedPositionInSamples(QMouseEvent *event, bool forceRoundingToPriorPosition = false) const;
    [[nodiscard]] juce::int64 calculatePositionInSamples(int x, bool rounded = false, bool forceRoundingToPriorPosition = false) const;
    [[nodiscard]] juce::int64 calculateSamplesInDivisionAmount() const;
    [[nodiscard]] QPointer<MidiNote> getNoteUnderCursor(QMouseEvent *event) const;

    QSharedPointer<Audio::MidiClip> mClip;
    QVector<QPointer<MidiNote>> mNotes;

    QPoint mClickedPos;

    bool mExtendingByTheRight = false;
    bool mExtendingByTheLeft = false;
    bool mMoving = false;
    QPoint mMovingDifferenceAmount;
    QPointer<MidiNote> mActionNoteReference;

    QRect mCurrentSelectionArea;
    SelectionOverlay mSelectionOverlay;

    QSharedPointer<TimelineProperties> mTimelineProperties;
};

} // Graphics

#endif //MIDIEDITOR_H

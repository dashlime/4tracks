#include "midiclippanel.h"

namespace Graphics
{

MidiClipPanel::MidiClipPanel(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent)
    : QWidget(parent), mClip(clip)
{
    setLayout(&mMainLayout);
    mMainLayout.setContentsMargins(90, 0, 0, 0);

    mMidiEditor = new MidiEditor(clip);
    mMidiNotesPanel = new MidiNotesPanel(clip);
    mVerticalScrollArea = new QScrollArea();
    mVerticalScrollWidget = new QWidget();

    double pixelsPerBeatAmount = mMidiEditor->calculatePixelsPerBeatAmount();
    double division = Utils::calculateDivision(pixelsPerBeatAmount);

    mDivisionsMarkerSpacer.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    mDivisionsMarkerSpacer.setFixedWidth(90);
    mDivisionsMarker = new DivisionsMarker(division, pixelsPerBeatAmount);

    connect(mMidiEditor, &MidiEditor::pixelsPerBeatAmountChanged, [=](double newPixelsPerBeatAmount) {
        mDivisionsMarker->refresh(Utils::calculateDivision(newPixelsPerBeatAmount), newPixelsPerBeatAmount);
    });

    mDivisionsMarkerContainer.setLayout(&mDivisionsMarkerLayout);
    mDivisionsMarkerLayout.addWidget(&mDivisionsMarkerSpacer);
    mDivisionsMarkerLayout.addWidget(mDivisionsMarker);
    mDivisionsMarkerLayout.setContentsMargins(0, 0, 0, 0);
    mDivisionsMarkerLayout.setSpacing(0);

    mVerticalScrollWidget->setLayout(&mVerticalScrollLayout);
    mVerticalScrollLayout.addWidget(mMidiNotesPanel);
    mVerticalScrollLayout.addWidget(mMidiEditor);
    mVerticalScrollLayout.setContentsMargins(0, 0, 0, 0);
    mVerticalScrollLayout.setSpacing(0);

    mVerticalScrollArea->setWidget(mVerticalScrollWidget);
    mVerticalScrollArea->setWidgetResizable(true);
    mVerticalScrollArea->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    mVerticalScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mVerticalScrollArea->horizontalScrollBar()->hide();
    mVerticalScrollArea->horizontalScrollBar()->setEnabled(false);
    mVerticalScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mMainLayout.addWidget(&mDivisionsMarkerContainer);
    mMainLayout.addWidget(mVerticalScrollArea);
}

void MidiClipPanel::paintEvent(QPaintEvent *event)
{}

} // Graphics
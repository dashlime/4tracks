#include "midiclippanel.h"

namespace Graphics
{

MidiClipPanel::MidiClipPanel(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent)
    : QWidget(parent), mClip(clip)
{
    setLayout(&mMainLayout);
    mMainLayout.setContentsMargins(2, 0, 0, 0);
    mMainLayout.setSpacing(0);

    mMidiEditor = new MidiEditor(clip);
    mMidiNotesPanel = new MidiNotesPanel(clip);
    mVerticalScrollArea = new QScrollArea();
    mVerticalScrollWidget = new QWidget();

    double pixelsPerBeatAmount = mMidiEditor->calculatePixelsPerBeatAmount();
    double division = Utils::calculateDivision(pixelsPerBeatAmount);
    mDivisionsMarker = new DivisionsMarker(division, pixelsPerBeatAmount);

    mDivisionsMarkerContainer.setLayout(&mDivisionsMarkerLayout);
    mDivisionsMarkerLayout.addWidget(mDivisionsMarker);
    mDivisionsMarkerLayout.addStretch(1);
    mDivisionsMarkerLayout.setContentsMargins(90, 0, 0, 0);

    connect(mMidiEditor, &MidiEditor::pixelsPerBeatAmountChanged, [=](double newPixelsPerBeatAmount) {
        mDivisionsMarker->refresh(Utils::calculateDivision(newPixelsPerBeatAmount, LARGE), newPixelsPerBeatAmount);
        mDivisionsMarker->setFixedWidth(mMidiEditor->width());
    });

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
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.fillRect(QRect(0, 0, 2, height()), QColor("#A1A1AA"));
}

} // Graphics
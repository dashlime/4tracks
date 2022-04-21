#include "midiclippanel.h"

namespace Graphics
{

MidiClipPanel::MidiClipPanel(const QSharedPointer<Audio::MidiClip> &clip, QWidget *parent)
    : QWidget(parent), mClip(clip)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    mTimelineProperties = QSharedPointer<TimelineProperties>::create(QSharedPointer<SelectionManager>::create());
    setLayout(&mMainLayout);
    mMainLayout.setContentsMargins(2, 0, 0, 0);
    mMainLayout.setSpacing(2);

    mMidiEditor = new MidiEditor(mTimelineProperties, clip);
    mMidiNotesPanel = new MidiNotesPanel(clip);
    mVerticalScrollArea = new QScrollArea();
    mVerticalScrollWidget = new QWidget();
    mVelocityPanel = new VelocityPanel(clip, mTimelineProperties);

    mMidiEditor->updatePixelsPerBeatAmount();
    double division = Utils::calculateDivision(mTimelineProperties->getPixelsPerBeatAmount());
    mDivisionsMarker = new DivisionsMarker(division, mTimelineProperties->getPixelsPerBeatAmount());

    mDivisionsMarkerContainer.setLayout(&mDivisionsMarkerLayout);
    mDivisionsMarkerLayout.addWidget(mDivisionsMarker);
    mDivisionsMarkerLayout.addStretch(1);
    mDivisionsMarkerLayout.setContentsMargins(90, 0, 0, 0);

    connect(mTimelineProperties.get(), &TimelineProperties::pixelsPerBeatAmountChanged, [=]()
    {
        mDivisionsMarker->refresh(Utils::calculateDivision(mTimelineProperties->getPixelsPerBeatAmount(), LARGE), mTimelineProperties->getPixelsPerBeatAmount());
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
    mVerticalScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mVerticalScrollArea->setFixedHeight(MINIMUM_MIDI_EDITOR_HEIGHT);
    mVerticalScrollArea->horizontalScrollBar()->hide();
    mVerticalScrollArea->horizontalScrollBar()->setEnabled(false);
    mVerticalScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mVelocityPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mVelocityPanel->setFixedHeight(DEFAULT_VELOCITY_PANEL_HEIGHT);

    mMainLayout.addWidget(&mDivisionsMarkerContainer);
    mMainLayout.addWidget(mVerticalScrollArea);
    mMainLayout.addWidget(mVelocityPanel);
}

QSharedPointer<Audio::MidiClip> MidiClipPanel::getAssociatedClip() const
{
    return mClip;
}

void MidiClipPanel::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    auto darkGrey = QColor(161, 161, 170);
    p.fillRect(QRect(0, 0, 2, height()), darkGrey);
    p.fillRect(QRect(0, mVerticalScrollArea->geometry().bottom() + 1, width(), 2), darkGrey);
}

} // Graphics
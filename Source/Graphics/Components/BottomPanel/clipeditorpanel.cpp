#include "clipeditorpanel.h"

namespace Graphics
{

ClipEditorPanel::ClipEditorPanel(const QSharedPointer<TimelineProperties> &properties, QWidget *parent)
    : QWidget(parent), mTimelineProperties(properties)
{
    connect(mTimelineProperties.get(), &TimelineProperties::currentSelectionChanged, [=]()
    {
        updateUI();
    });

    setFocusPolicy(Qt::ClickFocus);
    setMinimumHeight(DEFAULT_BOTTOM_PANEL_HEIGHT);
    setFixedHeight(DEFAULT_BOTTOM_PANEL_HEIGHT);

    setupComponents();
    setupLayouts();

    updateUI();
}

void ClipEditorPanel::setupLayouts()
{
    setLayout(&mMainLayout);
    mMainLayout.addWidget(&mClipPropertiesWidget);
    mMainLayout.setAlignment(&mClipPropertiesWidget, Qt::AlignLeft | Qt::AlignTop);
    mMainLayout.setContentsMargins(3, 3, 3, 3);

    mClipPropertiesWidget.setLayout(&mClipPropertiesLayout);
    mClipPropertiesWidget.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    mClipPropertiesWidget.setFixedWidth(150);

    mClipPropertiesLayout.addWidget(&mClipNameLabel);
    mClipPropertiesLayout.setContentsMargins(5, 5, 5, 5);
}

void ClipEditorPanel::setupComponents()
{
    auto font = QApplication::font();
    font.setPixelSize(12);
    font.setWeight(QFont::Weight::Medium);
    mClipNameLabel.setFont(font);

    mClipNameLabel.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mClipNameLabel.setFixedHeight(20);
}

void ClipEditorPanel::updateUI()
{
    if (mMidiClipPanel)
        mMidiClipPanel->deleteLater();

    auto currentSelection = mTimelineProperties->getCurrentSelection();

    if (currentSelection->getSelectionType() == SelectionManager::ClipsSelected
        && !currentSelection->getSelectedObjects().empty()) {
        if (currentSelection->getSelectedObjects().size() > 1) {
            mClipNameLabel.setText(QString::number(currentSelection->getSelectedObjects().size()) + " clips");
        }
        else {
            auto clip = ((Clip *) currentSelection->getSelectedObjects().at(0).get())->getClip();

            mClipNameLabel.setText(clip->getClipProperties()->getName());

            if (clip->getType() == Audio::Clip::MIDI_CLIP) {
                mMidiClipPanel = new MidiClipPanel(qSharedPointerCast<Audio::MidiClip>(clip));
                mMainLayout.addWidget(mMidiClipPanel);
                mMidiClipPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            }
        }
    }
    else {
        mClipNameLabel.setText("No clip selected");
    }
}

void ClipEditorPanel::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

} // Graphics
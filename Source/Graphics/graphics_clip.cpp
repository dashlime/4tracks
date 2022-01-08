#include "graphics_clip.h"
#include "ui_graphics_clip.h"

#include <QDebug>

namespace Graphics {

Clip::Clip(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Clip),
    mLabel("New clip")
{
    ui->setupUi(this);

    setLayout(ui->verticalLayout);

    setStyleSheet("Graphics--Clip { background-color: rgba(34, 197, 94, 50%); border: none; } Graphics--Clip:focus { border: 2px solid rgba(34, 197, 94, 100%); }");

    mLabel.setStyleSheet("background-color: rgba(34, 197, 94, 100%); padding: 2px; color: white;");
    ui->verticalLayout->insertWidget(0, &mLabel);
    ui->verticalLayoutWidget->setStyleSheet("background-color: rgba(0,0,0,0)");
    mLabel.setFixedHeight(20);
    mLabel.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

Clip::~Clip()
{
    delete ui;
}

void Clip::setSelected(bool isSelected)
{
    mSelected = isSelected;

    if (isSelected)
        setStyleSheet("Graphics--Clip { background-color: rgba(34, 197, 94, 50%); border: 2px solid rgba(34, 197, 94, 100%); }");

    else
        setStyleSheet("Graphics--Clip { background-color: rgba(34, 197, 94, 50%); border: none; }");

    repaint();
}

bool Clip::isSelected() const
{
    return mSelected;
}

void Clip::setClip(std::shared_ptr<Audio::Clip> clip)
{
    mClip = clip;

    mLabel.setText(clip->getName());

    if (mClip->getType() == Audio::Clip::AUDIO_CLIP)
        mAudioThumbnail.loadThumbnail(std::dynamic_pointer_cast<Audio::AudioClip>(clip), 512);

    update();
}

std::shared_ptr<Audio::Clip> Clip::getAudioClip()
{
    return mClip;
}

void Clip::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QColorConstants::Black);
    if (mClip->getType() == Audio::Clip::AUDIO_CLIP)
        mAudioThumbnail.drawThumbnail(p, QRect(0, 20, width(), height()-20));
}

void Clip::resizeEvent(QResizeEvent *event)
{
    ui->verticalLayout->setGeometry(geometry());
}

void Clip::focusInEvent(QFocusEvent *event)
{
}

void Clip::focusOutEvent(QFocusEvent *event)
{
}

} // namespace Graphics

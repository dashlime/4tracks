#include "clip.h"
#include "ui_clip.h"

#include <QDebug>

namespace Graphics
{

Clip::Clip(QWidget *parent)
    :
    QWidget(parent),
    ui(new Ui::Clip),
    mLabel("New clip")
{
    ui->setupUi(this);

    setLayout(ui->verticalLayout);

    setStyleSheet("Graphics--Clip { background-color: rgba(228, 228, 231, 50%); }");

    ui->verticalLayoutWidget->setStyleSheet("background-color: rgba(0,0,0,0)");
    ui->verticalLayout->insertWidget(0, &mLabel);

    mLabel.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mLabel.setFixedHeight(20);
    mLabel.setCursor(QCursor(Qt::CursorShape::OpenHandCursor));

    setSelected(false);
}

Clip::~Clip()
{
    delete ui;
}

void Clip::setSelected(bool isSelected)
{
    mSelected = isSelected;

    if (isSelected)
        mLabel.setStyleSheet("background-color: rgba(34, 197, 94, 50%); padding: 2px; color: white;");

    else
        mLabel.setStyleSheet("background-color: rgba(34, 197, 94, 100%); padding: 2px; color: white;");

    update();
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

std::shared_ptr<Audio::Clip> Clip::getClip()
{
    return mClip;
}

bool Clip::shouldMoveClip(QPoint mousePosition) const
{
    if (mLabel.geometry().contains(mousePosition - geometry().topLeft()))
        return true;

    return false;
}

void Clip::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    p.setPen(QColorConstants::Black);
    if (mClip->getType() == Audio::Clip::AUDIO_CLIP)
        mAudioThumbnail.drawThumbnail(p, QRect(0, 20, width(), height() - 20));
}

void Clip::resizeEvent(QResizeEvent *)
{
    ui->verticalLayout->setGeometry(QRect(0, 0, width(), height()));
    update();
}

} // namespace Graphics

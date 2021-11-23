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

    mLabel.setStyleSheet("background-color: #43A047; padding: 2px; color: white;");
    ui->verticalLayout->insertWidget(0, &mLabel);

    mLabel.setFixedHeight(20);
    mLabel.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

Clip::~Clip()
{
    delete ui;
}

void Clip::setClip(std::shared_ptr<Audio::Clip> clip)
{
    mClip = clip;

    mLabel.setText(clip->getName());
}

std::shared_ptr<Audio::Clip> Clip::getAudioClip()
{
    return mClip;
}

void Clip::resizeEvent(QResizeEvent *event)
{

}

} // namespace Graphics

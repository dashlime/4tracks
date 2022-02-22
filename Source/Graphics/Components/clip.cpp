#include "clip.h"
#include "ui_clip.h"

namespace Graphics
{

Clip::Clip(const QSharedPointer<ThumbnailManager> &thumbnailManager, QWidget *parent)
    :
    SelectableObject(parent),
    ui(new Ui::Clip),
    mLabel("New clip"),
    mThumbnailManager(thumbnailManager)
{
    ui->setupUi(this);

    setLayout(ui->verticalLayout);

    setStyleSheet("Graphics--Clip { background-color: rgba(228, 228, 231, 50%); }");

    ui->verticalLayoutWidget->setStyleSheet("background-color: rgba(0,0,0,0)");
    ui->verticalLayout->insertWidget(0, &mLabel);

    mLabel.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mLabel.setFixedHeight(20);
    mLabel.setCursor(QCursor(Qt::CursorShape::OpenHandCursor));

    setSelectedState(false);
}

Clip::~Clip()
{
    delete ui;
}

void Clip::setClip(const QSharedPointer<Audio::Clip> &clip)
{
    mClip = clip;

    mLabel.setText(clip->getClipProperties()->getName());

    if (mClip->getType() == Audio::Clip::AUDIO_CLIP)
        mThumbnailManager->registerAudioResource(qSharedPointerCast<Audio::AudioClip>(clip)->getAudioResource());

    connect(clip->getClipProperties().get(), &Audio::ClipProperties::startOffsetChanged, [=]()
    {
        update();
    });
    connect(clip->getClipProperties().get(), &Audio::ClipProperties::endOffsetChanged, [=]()
    {
        update();
    });
    update();
}

QSharedPointer<Audio::Clip> Clip::getClip()
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
        mThumbnailManager->drawThumbnailForClip(qSharedPointerCast<Audio::AudioClip>(mClip),
                                                p,
                                                QRect(0, 20, width(), height() - 20));
}

void Clip::resizeEvent(QResizeEvent *)
{
    ui->verticalLayout->setGeometry(QRect(0, 0, width(), height()));
    update();
}

void Clip::setSelectedState(bool isSelected)
{
    if (isSelected)
        mLabel.setStyleSheet("background-color: rgba(34, 197, 94, 50%); padding: 2px; color: white;");

    else
        mLabel.setStyleSheet("background-color: rgba(34, 197, 94, 100%); padding: 2px; color: white;");

    update();
}

Selection::SelectableObject::Type Clip::getType() const
{
    return Selection::SelectableObject::Clip;
}

} // namespace Graphics

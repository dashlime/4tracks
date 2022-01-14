#include "track.h"
#include "ui_track.h"

namespace Graphics
{

Track::Track(std::shared_ptr<Audio::Track> track, QWidget *parent)
    :
    QWidget(parent),
    ui(new Ui::Track),
    mAudioTrack(track)
{
    ui->setupUi(this);

    ui->trackName->setText(mAudioTrack->getName());

    setSelected(false);
    setFixedHeight(DEFAULT_TRACK_HEIGHT);
    setFixedWidth(150);

    ui->volumeSlider->setMinimum(0);
    ui->volumeSlider->setMaximum(200);
    ui->volumeSlider->setValue(mAudioTrack->getVolume() * 100);
    ui->volumeSlider->getValueLabel()
        ->setText(QString::number(juce::Decibels::gainToDecibels(double(ui->volumeSlider->value()) / 100), 'g', 2));

    connect(ui->volumeSlider, &QAbstractSlider::valueChanged, [=]()
    {
        mAudioTrack->setVolume(double(ui->volumeSlider->value()) / 100);
        ui->volumeSlider->getValueLabel()
            ->setText(QString::number(juce::Decibels::gainToDecibels(double(ui->volumeSlider->value()) / 100), 'g', 2));
    });

    ui->panSlider->setMinimum(-100);
    ui->panSlider->setMaximum(100);
    ui->panSlider->setValue(mAudioTrack->getPan() * 100);
    ui->panSlider->getValueLabel()
        ->setText(QString::number(double(ui->panSlider->value()) / 10) + (ui->panSlider->value() > 0 ? "R" : "L"));

    connect(ui->panSlider, &QAbstractSlider::valueChanged, [=]()
    {
        mAudioTrack->setPan(double(ui->panSlider->value()) / 100);
        ui->panSlider->getValueLabel()
            ->setText(QString::number(double(ui->panSlider->value()) / 10) + (ui->panSlider->value() > 0 ? "R" : "L"));
    });
}

Track::~Track()
{
    delete ui;
}

void Track::setSelected(bool isSelected)
{
    mSelected = isSelected;

    if (isSelected)
        setStyleSheet(
            "Graphics--Track { background-color: #E4E4E7; } QLabel { padding-left: 2px; color: white; background-color: rgba(34, 197, 94, 50%); }");

    else
        setStyleSheet(
            "Graphics--Track { background-color: #E4E4E7; } QLabel { padding-left: 2px; color: white; background-color: rgba(34, 197, 94, 100%);}");

    repaint();
}

bool Track::isSelected() const
{
    return mSelected;
}

void Track::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}

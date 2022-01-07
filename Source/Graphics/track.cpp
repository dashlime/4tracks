#include "track.h"
#include "ui_track.h"

namespace Graphics {

Track::Track(std::shared_ptr<Audio::AudioTrack> track, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Track),
    mAudioTrack(track)
{
    ui->setupUi(this);

    ui->trackName->setText(mAudioTrack->getName());

    setStyleSheet("Graphics--Track { background-color: rgba(34, 197, 94, 80%); } Graphics--Track:focus { background-color: rgba(34, 197, 94, 100%); } QLabel { color: white; }");
    setFixedHeight(DEFAULT_TRACK_HEIGHT);
    setFixedWidth(150);

    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    ui->volumeSlider->setMinimum(0);
    ui->volumeSlider->setMaximum(200);
    ui->volumeSlider->setValue(mAudioTrack->getVolume() * 100);
    ui->volumeSlider->getValueLabel()->setText(QString::number(juce::Decibels::gainToDecibels(double(ui->volumeSlider->value()) / 100), 'g', 2));
    connect(ui->volumeSlider, &QAbstractSlider::valueChanged, [=]() {
        mAudioTrack->setVolume(double(ui->volumeSlider->value()) / 100);
        ui->volumeSlider->getValueLabel()->setText(QString::number(juce::Decibels::gainToDecibels(double(ui->volumeSlider->value()) / 100), 'g', 2));
    });
}

Track::~Track()
{
    delete ui;
}

void Track::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

}

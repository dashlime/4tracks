#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->centralwidget->layout()->addWidget(&mUiTimeline);

    mTimeline = std::make_shared<Audio::Timeline>();
    mTimeline->setBpm(145);
    ui->bpmSpin->setRange(0, 512);
    ui->bpmSpin->setValue(145);

    std::shared_ptr<Audio::AudioTrack> track = std::make_shared<Audio::AudioTrack>("Audio Track", 0);
    if (track->addClip(std::make_shared<Audio::AudioClip>(track, "/media/alexis/084D-578B/Dev/4tracks/instru banger intense (bells).wav")) == false)
    {
        qDebug() << "Error when adding clip";
    }
    mTimeline->addTrack(track);

    mDeviceManager.initialise(0, 2, nullptr, false);
    auto setup = mDeviceManager.getAudioDeviceSetup();
    setup.bufferSize = 1024;
    setup.sampleRate = DEFAULT_SAMPLE_RATE;
    mDeviceManager.setAudioDeviceSetup(setup, true);

    if (mDeviceManager.getCurrentAudioDevice() != nullptr)
    {
        ui->playStopButton->setText("Stop");

        mPlayer.setSource(mTimeline.get());
        mDeviceManager.addAudioCallback(&mPlayer);

        mTimeline->prepareToPlay(mDeviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples(), DEFAULT_SAMPLE_RATE);

        mTimeline->play();
    }

    mUiTimeline.setAudioTimeline(mTimeline);

    connect(ui->zoomPlusButton, &QPushButton::clicked, [=]() {
        mUiTimeline.refreshZoomLevel(mUiTimeline.getZoomLevel() * 2);
    });

    connect(ui->zoomLessButton, &QPushButton::clicked, [=]() {
        mUiTimeline.refreshZoomLevel(mUiTimeline.getZoomLevel() / 2);
    });

    connect(ui->bpmSpin, &QSpinBox::valueChanged, [=]() {
        mTimeline->setBpm(ui->bpmSpin->value());
        mUiTimeline.refreshBpm();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


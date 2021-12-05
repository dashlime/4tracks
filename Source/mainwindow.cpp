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

    connect(ui->actionImport_file, &QAction::triggered, [=]() {
        importFile();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::importFile()
{
    QFileDialog fileDialog;
    fileDialog.setNameFilter("All audio files (*.wav)");
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (fileDialog.exec()) {
        if (fileDialog.selectedFiles().size() == 0)
            return;

        std::shared_ptr<Audio::AudioTrack> track = std::make_shared<Audio::AudioTrack>("Audio Track", 0);
        if (track->addClip(std::make_shared<Audio::AudioClip>(track, fileDialog.selectedFiles().at(0))) == false)
        {
            qDebug() << "Error when adding clip";
        }
        mTimeline->addTrack(track);

        mUiTimeline.displayTracks();
    }

}

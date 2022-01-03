#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString projectToLoad, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set default stylesheet
    setStyleSheet(QString::fromUtf8("QPushButton {"
                                        "background-color: #BDBDBD;"
                                        "border: 2px solid black;"
                                        "border-radius: 5px;"
                                        "color: black;"
                                        "padding-left: 5px;"
                                        "padding-right: 5px;"
                                    "}"

                                    "QPushButton:hover:!pressed {"
                                        "background-color: #9E9E9E;"
                                    "}"

                                    "QSpinBox {"
                                        "background-color: #BDBDBD;"
                                        "border: 2px solid black;"
                                        "border-radius: 5px;"
                                        "color: black;"
                                        "padding-right: 15px;"
                                    "}"

                                    "QLabel {"
                                        "color: black;"
                                    "}"

                                    "QScrollArea {"
                                    "   border: none;"
                                    "}"

                                    "QScrollBar:vertical {"
                                    "   background:rgba(0,0,0,0);"
                                    "   width:8px;    "
                                    "   margin: 0px 0px 0px 0px;"
                                    "}"

                                    "QScrollBar:horizontal {"
                                    "   background:rgba(0,0,0,0);"
                                    "   height:8px;    "
                                    "   margin: 0px 0px 0px 0px;"
                                    "}"

                                    "QScrollBar::handle {"
                                    "   background: #37474F;"
                                    "   border-radius: 4px;"
                                    "}"

                                    "QScrollBar::add-line, QScrollBar::sub-line {"
                                    "   background: none;"
                                    "}"

                                    "QScrollBar::left-arrow, QScrollBar::right-arrow, QScrollBar::down-arrow, QScrollBar::up-arrow, QScrollBar::add-page {"
                                    "   background: none;"
                                    "}"
    ));

    // set new project settings
    mProject = std::make_shared<Audio::Project>("Untitled Project");
    mProject->setBpm(120);
    ui->bpmSpin->setRange(0, 512);
    ui->bpmSpin->setValue(120);

    // initialise audio device
    mDeviceManager.initialise(0, 2, nullptr, false);
    auto setup = mDeviceManager.getAudioDeviceSetup();
    setup.bufferSize = 1024;
    setup.sampleRate = DEFAULT_SAMPLE_RATE;
    mDeviceManager.setAudioDeviceSetup(setup, true);

    if (mDeviceManager.getCurrentAudioDevice() != nullptr)
    {
        mPlayer.setSource(mProject.get());
        mDeviceManager.addAudioCallback(&mPlayer);

        mProject->prepareToPlay(mDeviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples(), DEFAULT_SAMPLE_RATE);
    }

    // init UI
    ui->centralwidget->layout()->addWidget(&mTimelineVerticalScrollView);
    mTimelineVerticalScrollView.setWidget(&mUiTimeline);
    mTimelineVerticalScrollView.setWidgetResizable(true);
    mTimelineVerticalScrollView.setFocusPolicy(Qt::FocusPolicy::NoFocus);
    mTimelineVerticalScrollView.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mUiTimeline.updateGeometry();

    mUiTimeline.setProject(mProject);

    ui->bpmSpin->setButtonSymbols(QSpinBox::NoButtons);

    for (auto button : findChildren<QPushButton*>())
    {
        button->setFocusPolicy(Qt::NoFocus);
    }

    updateTitle();

    // listen for changes in project
    mProject->savedStateChanged = [=]() {
        updateTitle();
    };

    mProject->trackAdded = [=]() {
        mUiTimeline.displayTracks();
        mProject->updateSavedState(Audio::Project::UNSAVED);
    };

    // connect main ui buttons / actions
    connect(ui->zoomPlusButton, &QPushButton::clicked, [=]() {
        mUiTimeline.refreshZoomLevel(mUiTimeline.getZoomLevel() * 2);
    });

    connect(ui->zoomLessButton, &QPushButton::clicked, [=]() {
        mUiTimeline.refreshZoomLevel(mUiTimeline.getZoomLevel() / 2);
    });

    connect(ui->bpmSpin, &QSpinBox::valueChanged, [=]() {
        mProject->setBpm(ui->bpmSpin->value());
        mUiTimeline.refreshBpm();
    });

    connect(ui->actionImport_file, &QAction::triggered, [=]() {
        importFile();
    });

    connect(ui->actionLoad_project, &QAction::triggered, [=]() {
        loadProject();
    });

    connect(ui->actionSave_project, &QAction::triggered, [=]() {
        saveProject();
    });

    connect(ui->playPauseButton, &QPushButton::clicked, [=]() {
        if (!mProject->isPlaying()) {
            mProject->play();
            ui->playPauseButton->setText("Pause");
        }
        else {
            mProject->pause();
            ui->playPauseButton->setText("Play");
        }
    });

    connect(ui->stopButton, &QPushButton::clicked, [=]() {
        mProject->stop();
        ui->playPauseButton->setText("Play");
    });

    // finally load project if explicilty asked
    if (projectToLoad != "")
    {
        loadProject(projectToLoad);
    }
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

        std::shared_ptr<Audio::AudioTrack> track = std::make_shared<Audio::AudioTrack>("Audio Track", mProject->getTracks().size());
        if (track->addClip(std::make_shared<Audio::AudioClip>(track, fileDialog.selectedFiles().at(0))) == false)
        {
            qDebug() << "Error when adding clip";
        }
        mProject->addTrack(track);
    }
}

void MainWindow::saveProject()
{
    if (currentProjectPath == "")
    {
        QFileDialog fileDialog;
        fileDialog.setFileMode(QFileDialog::Directory);
        fileDialog.setAcceptMode(QFileDialog::AcceptSave);
        if (fileDialog.exec()) {
            if (fileDialog.selectedFiles().size() == 0)
                return;

            ProjectSaver saver(mProject);
            saver.saveToDirectory(fileDialog.selectedFiles().first());

            mProject->updateSavedState(Audio::Project::SAVED);

            currentProjectPath = fileDialog.selectedFiles().first();
        }
    }
    else
    {
        ProjectSaver saver(mProject);
        saver.saveToDirectory(currentProjectPath);

        mProject->updateSavedState(Audio::Project::SAVED);
    }
}

void MainWindow::loadProject(QFile file)
{
    if (!file.exists())
    {
        QFileDialog fileDialog;
        fileDialog.setNameFilter("4tracks project files (*.4tpro)");
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        if (fileDialog.exec()) {
            if (fileDialog.selectedFiles().size() == 0)
                return;

            if (currentProjectPath == "" && mProject->getSavedState() == Audio::Project::SAVED)
            {
                ProjectSaver saver(mProject);
                saver.openProject(fileDialog.selectedFiles().first());
                mProject->updateSavedState(Audio::Project::SAVED);
                currentProjectPath = QFileInfo(fileDialog.selectedFiles().first()).absolutePath();
            }
            else
            {
                anotherInstanceRequired(fileDialog.selectedFiles().first());
            }
        }
    }

    else
    {
        ProjectSaver saver(mProject);
        saver.openProject(file.fileName());
        mProject->updateSavedState(Audio::Project::SAVED);
        currentProjectPath = file.fileName();
    }
}

void MainWindow::updateTitle()
{
    setWindowTitle((mProject->getSavedState() == Audio::Project::SAVED ? "" : "*") + mProject->getProjectName() + " - 4tracks");
}

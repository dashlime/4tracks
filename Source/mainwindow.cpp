#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString projectToLoad, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set default stylesheet
    QFont rubik("Roboto");

    QApplication::setFont(rubik);

    setStyleSheet(QString::fromUtf8("QMainWindow {"
                                        "background-color: white;"
                                    "}"

                                    "QPushButton {"
                                        "background-color: #D4D4D8;"
                                        "color: black;"
                                        "border: none;"
                                        "padding: 5px;"
                                    "}"

                                    "QPushButton:hover:!pressed:!checked {"
                                        "background-color: #E4E4E7;"
                                    "}"

                                    "QPushButton:pressed {"
                                        "background-color: #A1A1AA;"
                                    "}"
                                    "QPushButton:checked {"
                                        "background-color: #A1A1AA;"
                                    "}"

                                    "QSpinBox {"
                                        "background-color: #D4D4D8;"
                                        "border: none;"
                                        "height: 25px;"
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

                                    "Graphics--Timeline {"
                                        "background-color: #F4F4F5;"
                                        "padding: 5px;"
                                    "}"
    ));

    QFont font = ui->bpmSpin->font();
    font.setWeight(QFont::Medium);
    ui->bpmSpin->setFont(font);

    // set new project settings
    mProject = std::make_shared<Audio::Project>("Untitled Project");
    mProject->setBpm(120, false);
    ui->bpmSpin->setRange(0, 512);
    ui->bpmSpin->setValue(120);

    // initialise audio device
    reloadDeviceManager();

    // init UI
    ui->centralwidget->layout()->addWidget(&mTimelineVerticalScrollView);
    mTimelineVerticalScrollView.setWidget(&mUiTimeline);
    mTimelineVerticalScrollView.setWidgetResizable(true);
    mTimelineVerticalScrollView.setFocusPolicy(Qt::FocusPolicy::NoFocus);
    mTimelineVerticalScrollView.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mUiTimeline.updateGeometry();

    mUiTimeline.setProject(mProject);

    for (auto button : findChildren<QPushButton*>())
    {
        button->setFocusPolicy(Qt::NoFocus);
    }

    ui->bpmSpin->setButtonSymbols(QSpinBox::NoButtons);

    updateTitle();

    // listen for changes in project
    mProject->savedStateChanged = [=]() {
        updateTitle();
    };

    mProject->trackAdded = [=]() {
        mUiTimeline.displayTracks();
        mProject->updateSavedState(Audio::Project::UNSAVED);
    };

    mProject->bpmChanged = [=]() {
        ui->bpmSpin->setValue(mProject->getBpm());
    };

    // connect main ui buttons / actions
    connect(ui->zoomPlusButton, &QPushButton::clicked, [=]() {
        mUiTimeline.refreshZoomLevel(mUiTimeline.getZoomLevel() * 2);
    });

    connect(ui->zoomLessButton, &QPushButton::clicked, [=]() {
        mUiTimeline.refreshZoomLevel(mUiTimeline.getZoomLevel() / 2);
    });

    connect(ui->bpmSpin, &QSpinBox::valueChanged, [=]() {
        mProject->setBpm(ui->bpmSpin->value(), false);
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

    connect(ui->actionOptions, &QAction::triggered, [=]() {
        SettingsDialog *dialog = new SettingsDialog();
        dialog->setStyleSheet(styleSheet());
        dialog->show();
        dialog->settingsApplyed = [=]() {
            reloadDeviceManager();
        };
    });

    connect(ui->playButton, &QPushButton::toggled, [=](bool checked) {
        if (checked) {
            mProject->play();
        }
        else {
            mProject->pause();
        }
    });

    connect(ui->stopButton, &QPushButton::clicked, [=]() {
        mProject->stop();
        ui->playButton->setChecked(false);
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

void MainWindow::reloadDeviceManager()
{
    juce::File dataDir = juce::File(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
    juce::File audioSettings = dataDir.getChildFile("Preferences/audio.xml");

    if (audioSettings.exists())
    {
        mDeviceManager.initialise(0, 2, juce::parseXML(audioSettings).get(), false);
    }
    else
    {
        audioSettings.create();
        mDeviceManager.initialise(0, 2, nullptr, false);
    }

    if (mDeviceManager.getCurrentAudioDevice() != nullptr)
    {
        mPlayer.setSource(mProject.get());
        mDeviceManager.addAudioCallback(&mPlayer);

        mProject->prepareToPlay(mDeviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples(), DEFAULT_SAMPLE_RATE);
    }
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

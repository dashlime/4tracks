#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QString projectToLoad, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set default stylesheet
    QFont rubik("Roboto");

    QApplication::setFont(rubik);

    QFile mainStyleSheet(":/stylesheets/main.qss");
    mainStyleSheet.open(QIODevice::ReadOnly);

    setStyleSheet(mainStyleSheet.readAll());

    mainStyleSheet.close();

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
    ui->centralwidget->layout()->addWidget(&mUiTimeline);

    mUiTimeline.setProject(mProject);

    ui->bpmSpin->setButtonSymbols(QSpinBox::NoButtons);

    updateTitle();

    // listen for changes in project
    mProject->savedStateChanged = [=]()
    {
        updateTitle();
    };

    mProject->projectNameChanged = [=]()
    {
        updateTitle();
    };

    mProject->trackAdded = [=]()
    {
        mUiTimeline.displayTracks();
        mProject->updateSavedState(Audio::Project::UNSAVED);
    };

    mProject->bpmChanged = [=]()
    {
        ui->bpmSpin->setValue(mProject->getBpm());
    };

    connectUIActions();

    // finally load project if explicilty asked
    if (projectToLoad != "")
        loadProject(projectToLoad);

        // else add a default track to project
    else {
        mProject->addTrack(std::make_shared<Audio::Track>("Track", 0));
        mProject->updateSavedState(Audio::Project::SAVED);
    }

    mUiTimeline.setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectUIActions()
{
    connect(ui->zoomPlusButton, &QPushButton::clicked, [=]()
    {
        mUiTimeline.refreshZoomLevel(mUiTimeline.getZoomLevel() * 2);
    });

    connect(ui->zoomLessButton, &QPushButton::clicked, [=]()
    {
        mUiTimeline.refreshZoomLevel(mUiTimeline.getZoomLevel() / 2);
    });

    connect(ui->bpmSpin, &QSpinBox::valueChanged, [=]()
    {
        mProject->setBpm(ui->bpmSpin->value(), false);
        mUiTimeline.refreshBpm();
    });

    connect(ui->actionImport_file, &QAction::triggered, [=]()
    {
        importFile();
    });

    connect(ui->actionLoad_project, &QAction::triggered, [=]()
    {
        loadProject();
    });

    connect(ui->actionSave_project, &QAction::triggered, [=]()
    {
        saveProject();
    });

    connect(ui->actionOptions, &QAction::triggered, [=]()
    {
        SettingsDialog *dialog = new SettingsDialog();
        dialog->setStyleSheet(styleSheet());
        dialog->show();
        dialog->settingsApplyed = [=]()
        {
            reloadDeviceManager();
        };
    });

    connect(ui->actionAbout, &QAction::triggered, [=]()
    {
        QMessageBox *popup = new QMessageBox(this);
        popup->setWindowTitle("4tracks | About");
        popup->setText(
            "4tracks is an open-source DAW, written using Qt and JUCE libraries. You can find git repository at https://github.com/devanonyme-fr/4tracks\nCurrent version : 0.1 dev");
        popup->show();
    });

    connect(ui->playButton, &QPushButton::toggled, [=](bool checked)
    {
        if (checked) {
            mProject->play();
        }
        else {
            mProject->pause();
        }
    });

    connect(ui->stopButton, &QPushButton::clicked, [=]()
    {
        mProject->stop();
        ui->playButton->setChecked(false);
    });
}

void MainWindow::reloadDeviceManager()
{
    juce::File dataDir = juce::File(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
    juce::File audioSettings = dataDir.getChildFile("Preferences/audio.xml");

    if (audioSettings.exists()) {
        mDeviceManager.initialise(0, 2, juce::parseXML(audioSettings).get(), false);
    }
    else {
        audioSettings.create();
        mDeviceManager.initialise(0, 2, nullptr, false);
    }

    if (mDeviceManager.getCurrentAudioDevice() != nullptr) {
        mPlayer.setSource(mProject.get());
        mDeviceManager.addAudioCallback(&mPlayer);

        mProject
            ->prepareToPlay(mDeviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples(), DEFAULT_SAMPLE_RATE);
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

        // check if is there empty tracks to import file
        for (auto track : mProject->getTracks()) {
            if (track->getType() == Audio::Track::ANY_TRACK) {
                if (!track->addClip(std::make_shared<Audio::AudioClip>(track, fileDialog.selectedFiles().at(0)))) {
                    qDebug() << "Error when adding clip";
                }
                return;
            }
        }

        // else import it to a new track
        std::shared_ptr<Audio::Track>
            track = std::make_shared<Audio::Track>("Audio Track", mProject->getTracks().size());
        mProject->addTrack(track);
        if (!track->addClip(std::make_shared<Audio::AudioClip>(track, fileDialog.selectedFiles().at(0)))) {
            qDebug() << "Error when adding clip";
        }
    }
}

void MainWindow::saveProject()
{
    if (currentProjectPath == "") {
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
    else {
        ProjectSaver saver(mProject);
        saver.saveToDirectory(currentProjectPath);

        mProject->updateSavedState(Audio::Project::SAVED);
    }
}

void MainWindow::loadProject(QFile file)
{
    if (!file.exists()) {
        QFileDialog fileDialog;
        fileDialog.setNameFilter("4tracks project files (*.4tpro)");
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        if (fileDialog.exec()) {
            if (fileDialog.selectedFiles().size() == 0)
                return;

            if (currentProjectPath == "" && mProject->getSavedState() == Audio::Project::SAVED) {
                mProject->clearAllTracks();

                ProjectSaver saver(mProject);
                saver.openProject(fileDialog.selectedFiles().first());
                mProject->updateSavedState(Audio::Project::SAVED);
                currentProjectPath = QFileInfo(fileDialog.selectedFiles().first()).absolutePath();
            }
            else {
                anotherInstanceRequired(fileDialog.selectedFiles().first());
            }
        }
    }

    else {
        ProjectSaver saver(mProject);
        saver.openProject(file.fileName());
        mProject->updateSavedState(Audio::Project::SAVED);
        currentProjectPath = file.fileName();
    }
}

void MainWindow::updateTitle()
{
    setWindowTitle(
        (mProject->getSavedState() == Audio::Project::SAVED ? "" : "*") + mProject->getProjectName() + " - 4tracks");
}

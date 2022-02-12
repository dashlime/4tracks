#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace Graphics
{

MainWindow::MainWindow(const QString &projectToLoad, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), mProject(QSharedPointer<Audio::Project>::create("Untitled Project")),
      mUiTimeline(mProject), mTimelineScrollWidget(&mUiTimeline, mProject)
{
    ui->setupUi(this);

    setupStylesheet();

    ui->timelineNavigationLayout->insertWidget(1, &mTimelineScrollWidget);
    ui->timelineNavigationLayout->setStretch(1, 1);

    // set new project settings
    mProject->getProjectProperties()->setBpm(120, false);
    ui->bpmSpin->setRange(0, 512);
    ui->bpmSpin->setValue(120);

    // initialise audio device
    reloadDeviceManager();

    // init UI
    ui->centralwidget->layout()->addWidget(&mUiTimeline);

    updateTitle();

    setupProjectCallbacks();
    setupUICallbacks();

    // finally, load project if explicitly asked
    if (projectToLoad != "")
        loadProject(projectToLoad);

        // else add a default track to project
    else {
        mProject->createTrack("Track");
        mProject->getProjectProperties()->updateSavedState(Audio::ProjectProperties::SAVED);
    }

    mUiTimeline.setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupStylesheet()
{
    QFont roboto("Roboto");

    QApplication::setFont(roboto);

    QFile mainStyleSheet(":/stylesheets/main.qss");
    mainStyleSheet.open(QIODevice::ReadOnly);

    setStyleSheet(mainStyleSheet.readAll());

    mainStyleSheet.close();

    QFont font = ui->bpmSpin->font();
    font.setWeight(QFont::Medium);
    ui->bpmSpin->setFont(font);
    ui->bpmSpin->setButtonSymbols(QSpinBox::NoButtons);
}

void MainWindow::setupProjectCallbacks()
{
    connect(mProject->getProjectProperties().get(), &Audio::ProjectProperties::savedStateChanged, [=]()
    {
        updateTitle();
    });

    connect(mProject->getProjectProperties().get(), &Audio::ProjectProperties::projectNameChanged, [=]()
    {
        updateTitle();
    });

    connect(mProject->getProjectProperties().get(), &Audio::ProjectProperties::bpmChanged, [=]()
    {
        ui->bpmSpin->setValue((int) mProject->getProjectProperties()->getBpm());
    });
}

void MainWindow::setupUICallbacks()
{
    connect(ui->zoomPlusButton, &QPushButton::clicked, [=]()
    {
        mUiTimeline.getTimelineProperties()->setZoomLevel(mUiTimeline.getTimelineProperties()->getZoomLevel() * 2);
    });

    connect(ui->zoomLessButton, &QPushButton::clicked, [=]()
    {
        mUiTimeline.getTimelineProperties()->setZoomLevel(mUiTimeline.getTimelineProperties()->getZoomLevel() / 2);
    });

    connect(ui->bpmSpin, &QSpinBox::valueChanged, [=]()
    {
        mProject->getProjectProperties()->setBpm(ui->bpmSpin->value(), true);
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
        auto *dialog = new SettingsDialog();
        dialog->setStyleSheet(styleSheet());
        dialog->setAttribute(Qt::WA_DeleteOnClose, true);
        dialog->show();
        connect(dialog, &SettingsDialog::settingsApplied, [=]()
        {
            reloadDeviceManager();
        });
    });

    connect(ui->actionAbout, &QAction::triggered, [=]()
    {
        auto *popup = new QMessageBox(this);
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

    QString nameFilter = "All audio files (";
    for (const QString &name: SUPPORTED_AUDIO_FILENAMES) {
        nameFilter +="*." + name + " ";
    }
    // remove last space
    nameFilter.chop(1);
    nameFilter += ")";

    fileDialog.setNameFilter(nameFilter);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (fileDialog.exec()) {
        if (fileDialog.selectedFiles().empty())
            return;

        // check if is there empty tracks to import file
        for (const auto &track: mProject->getTracks()) {
            if (track->getType() == Audio::Track::ANY_TRACK) {
                if (mProject->createAudioClip(track, fileDialog.selectedFiles().at(0)) == -1) {
                    qDebug() << "Error when adding clip";
                }
                return;
            }
        }

        // else import it to a new track
        int trackID = mProject->createTrack("Audio Track");
        auto track = mProject->getTrackByIndex(trackID);
        if (mProject->createAudioClip(track, fileDialog.selectedFiles().at(0)) == -1) {
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
            if (fileDialog.selectedFiles().empty())
                return;

            ProjectSaver saver(mProject);
            saver.saveToDirectory(fileDialog.selectedFiles().first());

            mProject->getProjectProperties()->updateSavedState(Audio::ProjectProperties::SAVED);

            currentProjectPath = fileDialog.selectedFiles().first();
        }
    }
    else {
        ProjectSaver saver(mProject);
        saver.saveToDirectory(currentProjectPath);

        mProject->getProjectProperties()->updateSavedState(Audio::ProjectProperties::SAVED);
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
            if (fileDialog.selectedFiles().empty())
                return;

            if (currentProjectPath == ""
                && mProject->getProjectProperties()->getSavedState() == Audio::ProjectProperties::SAVED) {
                mProject->clearAllTracks();

                ProjectSaver saver(mProject);
                saver.openProject(fileDialog.selectedFiles().first());
                mProject->getProjectProperties()->updateSavedState(Audio::ProjectProperties::SAVED);
                currentProjectPath = QFileInfo(fileDialog.selectedFiles().first()).absolutePath();
            }
            else {
                emit anotherInstanceRequired(fileDialog.selectedFiles().first());
            }
        }
    }

    else {
        ProjectSaver saver(mProject);
        saver.openProject(file.fileName());
        mProject->getProjectProperties()->updateSavedState(Audio::ProjectProperties::SAVED);
        currentProjectPath = file.fileName();
    }
}

void MainWindow::updateTitle()
{
    setWindowTitle(
        (mProject->getProjectProperties()->getSavedState() == Audio::ProjectProperties::SAVED ? "" : "*")
            + mProject->getProjectProperties()->getProjectName() + " - 4tracks");
}

}
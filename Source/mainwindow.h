#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "Graphics/graphics_timeline.h"
#include "Audio/project.h"
#include "Audio/JuceIncludes.h"
#include "Graphics/settingsdialog.h"
#include "Utils/projectsaver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString projectToLoad = "", QWidget *parent = nullptr);
    ~MainWindow();

    void reloadDeviceManager();

    void importFile();
    void saveProject();
    void loadProject(QFile file = QFile());

    void updateTitle();

    std::function<void(QString)> anotherInstanceRequired;

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Audio::Project> mProject;
    Graphics::Timeline mUiTimeline;
    QScrollArea mTimelineVerticalScrollView;
    juce::AudioSourcePlayer mPlayer;
    juce::AudioDeviceManager mDeviceManager;

    QString currentProjectPath = "";
};
#endif // MAINWINDOW_H

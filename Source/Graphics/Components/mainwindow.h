#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

#include "timeline.h"
#include "Audio/Project/project.h"
#include "Audio/JuceIncludes.h"
#include "Graphics/Dialogs/settingsdialog.h"
#include "Utils/projectsaver.h"

namespace Graphics
{

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    MainWindow(const QString &projectToLoad = "", QWidget *parent = nullptr);
    ~MainWindow();

    void setupStylesheet();
    void setupProjectCallbacks();
    void setupUICallbacks();

    void reloadDeviceManager();

    void importFile();
    void saveProject();
    void loadProject(QFile file = QFile());

    void updateTitle();

signals:
    void anotherInstanceRequired(QString projectPath);

private:
    Ui::MainWindow *ui;
    QSharedPointer<Audio::Project> mProject;
    Timeline mUiTimeline;
    juce::AudioSourcePlayer mPlayer;
    juce::AudioDeviceManager mDeviceManager;

    QString currentProjectPath = "";
};

}

#endif // MAINWINDOW_H

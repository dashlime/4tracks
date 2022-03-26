#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

#include "Graphics/Components/Timeline/timeline.h"
#include "Audio/Project/project.h"
#include "Audio/JuceIncludes.h"
#include "Graphics/Windows/settingsdialog.h"
#include "Audio/Project/projectsaver.h"
#include "Graphics/Components/Timeline/timelinescrollwidget.h"
#include "Graphics/Components/BottomPanel/clipeditorpanel.h"

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
    ClipEditorPanel mClipEditorPanel;
    TimelineScrollWidget mTimelineScrollWidget;
    juce::AudioSourcePlayer mPlayer;
    juce::AudioDeviceManager mDeviceManager;

    QString currentProjectPath = "";
};

}

#endif // MAINWINDOW_H

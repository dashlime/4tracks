#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Graphics/graphics_timeline.h"
#include "Audio/timeline.h"
#include "Audio/JuceIncludes.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Audio::Timeline> mTimeline;
    Graphics::Timeline mUiTimeline;
    juce::AudioSourcePlayer mPlayer;
    juce::AudioDeviceManager mDeviceManager;
};
#endif // MAINWINDOW_H

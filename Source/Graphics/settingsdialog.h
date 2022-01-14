#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtWidgets>
#include <QStandardPaths>

#include "Audio/JuceIncludes.h"

namespace Ui
{
class SettingsDialog;
}

class SettingsDialog: public QDialog
{
Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    void connectUIElements();
    void refreshUI();

    std::function<void()> settingsApplyed;

private:
    Ui::SettingsDialog *ui;
    juce::AudioDeviceManager mDeviceManager;
    juce::AudioDeviceManager::AudioDeviceSetup mAudioDeviceSetup;
    juce::AudioDeviceManager::AudioDeviceSetup mLastSetup;
};

#endif // SETTINGSDIALOG_H

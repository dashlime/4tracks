#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowTitle("4tracks | Options");

    juce::File dataDir = juce::File(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString());
    juce::File audioSettings = dataDir.getChildFile("Preferences/audio.xml");

    if (audioSettings.exists()) {
        mDeviceManager.initialise(0, 2, juce::parseXML(audioSettings).get(), false);
    }
    else {
        audioSettings.create();
        mDeviceManager.initialise(0, 2, nullptr, false);
    }

    refreshUI();

    connect(ui->applyButton, &QPushButton::clicked, [=]()
    {
        if (mDeviceManager.createStateXml() != nullptr)
            mDeviceManager.createStateXml()->writeTo(audioSettings);

        settingsApplyed();
        close();
    });

    connect(ui->cancelButton, &QPushButton::clicked, [=]()
    {
        close();
    });

    connectUIElements();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::connectUIElements()
{
    connect(ui->audioDriverComboBox, &QComboBox::currentTextChanged, [=](const QString& driver)
    {
        if (driver.isEmpty())
            return;

        mDeviceManager.setCurrentAudioDeviceType(driver.toStdString(), true);
        refreshUI();
    });

    connect(ui->inputDeviceComboBox, &QComboBox::currentTextChanged, [=](const QString& device)
    {
        if (device.isEmpty())
            return;

        mAudioDeviceSetup.inputDeviceName = device.toStdString();

        juce::String error = mDeviceManager.setAudioDeviceSetup(mAudioDeviceSetup, true);
        if (error != "") {
            qDebug() << "Error when changing input device : " << error.toStdString().c_str();
            mDeviceManager.setAudioDeviceSetup(mLastSetup, true);
        }

        refreshUI();
    });

    connect(ui->outputDeviceComboBox, &QComboBox::currentTextChanged, [=](const QString& device)
    {
        if (device.isEmpty())
            return;

        mAudioDeviceSetup.outputDeviceName = device.toStdString();

        juce::String error = mDeviceManager.setAudioDeviceSetup(mAudioDeviceSetup, true);
        if (error != "") {
            qDebug() << "Error when changing output device : " << error.toStdString().c_str();
            mDeviceManager.setAudioDeviceSetup(mLastSetup, true);
        }

        refreshUI();
    });

    connect(ui->sampleRateComboBox, &QComboBox::currentTextChanged, [=](const QString& sampleRate)
    {
        if (sampleRate.isEmpty())
            return;

        mAudioDeviceSetup.sampleRate = sampleRate.toInt();

        juce::String error = mDeviceManager.setAudioDeviceSetup(mAudioDeviceSetup, true);
        if (error != "") {
            qDebug() << "Error when changing sample rate : " << error.toStdString().c_str();
            mDeviceManager.setAudioDeviceSetup(mLastSetup, true);
        }

        refreshUI();
    });

    connect(ui->bufferSizeComboBox, &QComboBox::currentTextChanged, [=](const QString& bufferSize)
    {
        if (bufferSize.isEmpty())
            return;

        mAudioDeviceSetup.bufferSize = bufferSize.toInt();

        juce::String error = mDeviceManager.setAudioDeviceSetup(mAudioDeviceSetup, true);
        if (error != "") {
            qDebug() << "Error when changing buffer size : " << error.toStdString().c_str();
            mDeviceManager.setAudioDeviceSetup(mLastSetup, true);
        }

        refreshUI();
    });
}

void SettingsDialog::refreshUI()
{
    QComboBox *comboBoxes[] = {ui->audioDriverComboBox, ui->inputDeviceComboBox, ui->outputDeviceComboBox,
                               ui->sampleRateComboBox, ui->bufferSizeComboBox};

    mAudioDeviceSetup = mDeviceManager.getAudioDeviceSetup();
    mLastSetup = mAudioDeviceSetup;

    for (auto comboBox : comboBoxes) {
        comboBox->blockSignals(true);
        comboBox->clear();
    }

    // audio driver
    for (int i = 0; i < mDeviceManager.getAvailableDeviceTypes().size(); i++) {
        ui->audioDriverComboBox
            ->addItem(mDeviceManager.getAvailableDeviceTypes().getUnchecked(i)->getTypeName().toStdString().c_str());
    }
    ui->audioDriverComboBox->setCurrentText(mDeviceManager.getCurrentAudioDeviceType().toStdString().c_str());

    // input devices
    for (const auto& device : mDeviceManager.getCurrentDeviceTypeObject()->getDeviceNames(true)) {
        ui->inputDeviceComboBox->addItem(device.toStdString().c_str());
    }
    ui->inputDeviceComboBox->setCurrentText(mDeviceManager.getAudioDeviceSetup().inputDeviceName.toStdString().c_str());

    // output devices
    for (const auto& device : mDeviceManager.getCurrentDeviceTypeObject()->getDeviceNames(false)) {
        ui->outputDeviceComboBox->addItem(device.toStdString().c_str());
    }
    ui->outputDeviceComboBox
        ->setCurrentText(mDeviceManager.getAudioDeviceSetup().outputDeviceName.toStdString().c_str());

    // sample rate
    for (auto sampleRate : mDeviceManager.getCurrentAudioDevice()->getAvailableSampleRates()) {
        ui->sampleRateComboBox->addItem(QString::number(sampleRate));
    }
    ui->sampleRateComboBox
        ->setCurrentText(QString::number(mDeviceManager.getCurrentAudioDevice()->getCurrentSampleRate()));

    // sample rate
    for (auto bufferSize : mDeviceManager.getCurrentAudioDevice()->getAvailableBufferSizes()) {
        ui->bufferSizeComboBox->addItem(QString::number(bufferSize));
    }
    ui->bufferSizeComboBox
        ->setCurrentText(QString::number(mDeviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples()));

    for (auto comboBox : comboBoxes) {
        comboBox->blockSignals(false);
    }
}

#include <QtTest>
#include <QCoreApplication>

#include "Audio/track.h"
#include "Audio/midiclip.h"
#include "Audio/audioclip.h"

class AudioTrackTest : public QObject
{
    Q_OBJECT

public:
    AudioTrackTest();
    ~AudioTrackTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void getTypeTest();

};

AudioTrackTest::AudioTrackTest() {}

AudioTrackTest::~AudioTrackTest() {}

void AudioTrackTest::initTestCase()
{

}

void AudioTrackTest::cleanupTestCase()
{

}

void AudioTrackTest::getTypeTest()
{
    auto t = std::make_shared<Audio::Track>("Test track", 0);
    QVERIFY(t->getType() == Audio::Track::ANY_TRACK);

    t->addClip(std::make_shared<Audio::AudioClip>(t, ""));
    QVERIFY(t->getType() == Audio::Track::AUDIO_TRACK);

    /*t.reset();
    t = std::make_shared<Audio::Track>("Test track", 0);
    t->addClip(std::make_shared<Audio::MidiClip>(t));
    QVERIFY(t->getType() == Audio::Track::MIDI_TRACK);*/
}

QTEST_MAIN(AudioTrackTest)

#include "tst_audiotrack.moc"

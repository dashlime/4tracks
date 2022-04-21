#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QVector>

#define NOTES_IN_OCTAVE 12
#define START_MIDI_OCTAVE -1
#define END_MIDI_OCTAVE 8

#define MIDI_NOTE_MINIMUM_LENGTH_IN_SAMPLES 500

#define DEFAULT_MIDI_CHANNEL 1

static const QVector<bool>
    MAJOR_SCALE_SCHEME = {true, false, true, false, true, true, false, true, false, true, false, true};

#define DEFAULT_SAMPLE_RATE 48000
#define DEFAULT_PIXEL_PER_BEAT_AMOUNT 2
enum GRID_SIZE_TYPE
{
    DEFAULT, LARGE
};

static const QVector<int> MINIMUM_SPACE_BETWEEN_GRID_LINES = {30, 60};

// divisions : 1/16, 1/8, 1/4, 1/2, 1, 2, ...
static const QVector<double> DEFAULT_DIVISIONS = {1 / 16.f, 1 / 8.f, 1 / 4.f, 1 / 2.f, 1, 2, 4, 8, 16, 32, 64};

#define MP3 "mp3"
#define WAV "wav"
#define AIFF "aiff"

const static QStringList SUPPORTED_AUDIO_FILENAMES = {MP3, WAV, AIFF};

#define DEFAULT_TRACK_HEIGHT 120
#define DEFAULT_BOTTOM_PANEL_HEIGHT 200
#define DEFAULT_VELOCITY_PANEL_HEIGHT 120
#define MINIMUM_MIDI_EDITOR_HEIGHT 160
#define DEFAULT_NOTE_HEIGHT 20
#define DEFAULT_MIDI_EDITOR_LEFT_PANEL_WIDTH 90

#endif // CONSTANTS_H

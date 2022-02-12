#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QVector>

#define DEFAULT_SAMPLE_RATE 48000
#define DEFAULT_PIXEL_PER_BEAT_AMOUNT 2
#define MINIMUM_SPACE_BETWEEN_GRID_LINES 30

// divisions : 1/16, 1/8, 1/4, 1/2, 1, 2, ...
static const QVector<double> DEFAULT_DIVISIONS = {1 / 16.f, 1 / 8.f, 1 / 4.f, 1 / 2.f, 1, 2, 4, 8, 16, 32, 64};

#define MP3 "mp3"
#define WAV "wav"
#define AIFF "aiff"

const static QStringList SUPPORTED_AUDIO_FILENAMES = {MP3, WAV, AIFF};

#define DEFAULT_TRACK_HEIGHT 120

#endif // CONSTANTS_H

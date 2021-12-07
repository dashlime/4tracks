#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <vector>

#define DEFAULT_SAMPLE_RATE 48000
#define DEFAULT_PIXEL_PER_BEAT_AMOUNT 2
#define MINIMUM_SPACE_BETWEEN_GRID_LINES 30

// divisions : 1/16, 1/8, 1/4, 1/2, 1, 2, ...
static const std::vector<double> DEFAULT_DIVISIONS = {1/16, 1/8, 1/4, 1/2, 1, 2, 4, 8, 16, 32, 64};

#define DEFAULT_TRACK_HEIGHT 120

#endif // CONSTANTS_H

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <cmath>

#include <QLayout>
#include <QWidget>

#include "Audio/JuceIncludes.h"
#include "Constants.h"

class Utils
{
public:
    static long search_closest(const QVector<double> &sorted_array, double x);

    static void clearLayout(QLayout *layout, bool deleteWidget = false);

    static double calculateDivision(double pixelsPerBeatAmount, GRID_SIZE_TYPE gridSizeType = DEFAULT);
    static juce::int64 roundPosition(juce::int64 positionInSamples,
                                     double pixelsPerBeatAmount,
                                     double bpm,
                                     bool forceRoundingToPriorPosition = false,
                                     GRID_SIZE_TYPE gridSizeType = DEFAULT);

    static int samplesToPixels(juce::int64 samples, double pixelsPerBeatAmount, double bpm);
    static juce::int64 pixelsToSamples(int pixels, double pixelsPerBeatAmount, double bpm);
};

#endif // UTILS_H

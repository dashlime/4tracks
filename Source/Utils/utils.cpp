#include "utils.h"

long Utils::search_closest(const QVector<double> &sorted_array, double x)
{
    auto iter_geq = std::lower_bound(
        sorted_array.begin(),
        sorted_array.end(),
        x
    );

    if (iter_geq == sorted_array.begin()) {
        return 0;
    }

    double a = *(iter_geq - 1);
    double b = *(iter_geq);

    if (fabs(x - a) < fabs(x - b)) {
        return iter_geq - sorted_array.begin() - 1;
    }

    return iter_geq - sorted_array.begin();
}

void Utils::clearLayout(QLayout *layout, bool deleteWidget)
{
    if (layout == nullptr)
        return;
    QLayoutItem *item;
    while ((item = layout->takeAt(0))) {
        if (item->layout()) {
            clearLayout(item->layout());
            delete item->layout();
        }
        if (item->widget() && deleteWidget) {
            delete item->widget();
        }
        delete item;
    }
}

double Utils::calculateDivision(double pixelsPerBeatAmount, GRID_SIZE_TYPE gridSizeType)
{
    double division = (double) MINIMUM_SPACE_BETWEEN_GRID_LINES[gridSizeType] / pixelsPerBeatAmount;
    int index = (int) Utils::search_closest(DEFAULT_DIVISIONS, division);
    division = DEFAULT_DIVISIONS[index];
    return division;
}

juce::int64 Utils::roundPosition(juce::int64 positionInSamples,
                                 double pixelsPerBeatAmount,
                                 double bpm,
                                 bool forceRoundingToPriorPosition,
                                 GRID_SIZE_TYPE gridSizeType)
{
    int samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;

    double
        samplesInDivision = calculateDivision(pixelsPerBeatAmount, gridSizeType) / bpm * (double) samplesPerMinute;

    juce::int64 result;
    if (!forceRoundingToPriorPosition)
        result = juce::int64(round((double) positionInSamples / samplesInDivision) * samplesInDivision);
    else
        result = juce::int64(floor((double) positionInSamples / samplesInDivision) * samplesInDivision);

    return result;
}

int Utils::samplesToPixels(juce::int64 samples, double pixelsPerBeatAmount, double bpm)
{
    int samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    double pixelsPerMinute = bpm * pixelsPerBeatAmount;

    return int((double) samples / (double) samplesPerMinute * pixelsPerMinute);
}

juce::int64 Utils::pixelsToSamples(int pixels, double pixelsPerBeatAmount, double bpm)
{
    int samplesPerMinute = DEFAULT_SAMPLE_RATE * 60;
    double samplesPerPixel = (double) samplesPerMinute / bpm / pixelsPerBeatAmount;

    return juce::int64(samplesPerPixel * pixels);
}
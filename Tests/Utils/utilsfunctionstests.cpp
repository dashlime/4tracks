#include "utilstests.h"

TEST_F(UtilsTests, SearchClosest)
{
    QVector<double> array = {-2, -0.5, 0, 2, 5.5, 18};

    EXPECT_EQ(array[Utils::search_closest(array, -1.5)], -2);
    EXPECT_EQ(array[Utils::search_closest(array, 0.5)], 0);
    EXPECT_EQ(array[Utils::search_closest(array, 5)], 5.5);
}

TEST_F(UtilsTests, CalculateDivision)
{
    int pixelsPerBeatAmount = 20;
    EXPECT_EQ(Utils::calculateDivision(pixelsPerBeatAmount), 2);

    pixelsPerBeatAmount = 10;
    EXPECT_EQ(Utils::calculateDivision(pixelsPerBeatAmount), 4);

    pixelsPerBeatAmount = 60;
    EXPECT_EQ(Utils::calculateDivision(pixelsPerBeatAmount), 1 / 2.f);
}

TEST_F(UtilsTests, RoundPosition)
{
    int bpm = 120;
    int pixelsPerBeatAmount = 20;
    int pixels = 500;
    juce::int64 samples = Utils::pixelsToSamples(pixels, pixelsPerBeatAmount, bpm);
    juce::int64 roundedSamples = Utils::roundPosition(samples, pixelsPerBeatAmount, bpm);

    double division = Utils::calculateDivision(pixelsPerBeatAmount);
    double samplesInDivision = division / bpm * (DEFAULT_SAMPLE_RATE * 60);
    auto result = round((double) samples / samplesInDivision) * samplesInDivision;

    EXPECT_EQ(result, roundedSamples);

    roundedSamples = Utils::roundPosition(samples, pixelsPerBeatAmount, bpm, true);
    result = floor((double) samples / samplesInDivision) * samplesInDivision;
    EXPECT_EQ(result, roundedSamples);
}

TEST_F(UtilsTests, PixelsAndSamplesConversion)
{
    int pixels = 500;
    int bpm = 120;
    int pixelsPerBeatAmount = 20;

    juce::int64 samples = Utils::pixelsToSamples(pixels, pixelsPerBeatAmount, bpm);
    EXPECT_EQ(Utils::samplesToPixels(samples, pixelsPerBeatAmount, bpm), pixels);
}
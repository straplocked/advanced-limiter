/*
 * Advanced Limiter - Open-source brickwall limiter plugin
 * Copyright (C) 2024-2026 Advanced Limiter contributors
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "LevelMeterData.h"
#include <cmath>
#include <algorithm>

void LevelMeterData::prepare (double sampleRate)
{
    currentSampleRate = sampleRate;

    meterBuffer.resize (1);

    loudnessMeter.prepare (sampleRate, 2);
    lraMeter.prepare (sampleRate);
    truePeakMeter.prepare (sampleRate);

    reset();
}

void LevelMeterData::reset()
{
    currentFrame = LimiterMeterFrame {};
    accumulatedPeakGR = 0.0f;
    accumulatedAvgGR = 0.0f;
    accumulatedClipCount = 0;

    loudnessMeter.reset();
    lraMeter.reset();
    truePeakMeter.reset();
}

void LevelMeterData::pushInputLevel (const juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();

    if (buffer.getNumChannels() >= 1)
        currentFrame.inputL = computePeakDB (buffer.getReadPointer (0), numSamples);

    if (buffer.getNumChannels() >= 2)
        currentFrame.inputR = computePeakDB (buffer.getReadPointer (1), numSamples);
    else
        currentFrame.inputR = currentFrame.inputL;
}

void LevelMeterData::pushOutputLevel (const juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();

    // Compute output peak dB per channel
    if (numChannels >= 1)
        currentFrame.outputL = computePeakDB (buffer.getReadPointer (0), numSamples);

    if (numChannels >= 2)
        currentFrame.outputR = computePeakDB (buffer.getReadPointer (1), numSamples);
    else
        currentFrame.outputR = currentFrame.outputL;

    // Feed loudness meter with output audio
    const float* const* channelData = buffer.getArrayOfReadPointers();
    loudnessMeter.pushBlock (channelData, numChannels, numSamples);

    // Get LUFS values
    currentFrame.momentaryLUFS = (float) loudnessMeter.getMomentaryLUFS();
    currentFrame.shortTermLUFS = (float) loudnessMeter.getShortTermLUFS();
    currentFrame.integratedLUFS = (float) loudnessMeter.getIntegratedLUFS();

    // Push short-term LUFS to LRA meter
    lraMeter.pushShortTermLUFS (loudnessMeter.getShortTermLUFS());
    currentFrame.lra = lraMeter.getLRA();

    // Publish the completed frame
    publishFrame();
}

void LevelMeterData::pushGainReduction (float grL, float grR)
{
    currentFrame.grL = grL;
    currentFrame.grR = grR;

    // Track peak GR (most negative = deepest reduction)
    float blockGR = std::min (grL, grR);
    accumulatedPeakGR = std::min (accumulatedPeakGR, blockGR);

    // EMA of block GR (alpha ~0.01 gives ~3 second averaging at 30fps block rate)
    constexpr float kGRAvgAlpha = 0.01f;
    accumulatedAvgGR = accumulatedAvgGR + kGRAvgAlpha * (blockGR - accumulatedAvgGR);

    // Write accumulated stats into the frame
    currentFrame.peakGR = accumulatedPeakGR;
    currentFrame.avgGR = accumulatedAvgGR;
}

void LevelMeterData::pushTruePeakSample (float sample, int channel)
{
    truePeakMeter.processSample (sample, channel);

    currentFrame.truePeakL = truePeakMeter.getTruePeakDB (0);
    currentFrame.truePeakR = truePeakMeter.getTruePeakDB (1);
}

void LevelMeterData::resetLoudness()
{
    loudnessMeter.reset();
    lraMeter.reset();
}

void LevelMeterData::pushClipSample (float sample, float ceilingLinear)
{
    if (std::fabs (sample) >= ceilingLinear - 1e-7f)
        accumulatedClipCount++;

    currentFrame.clipCount = accumulatedClipCount;
}

void LevelMeterData::resetStatistics()
{
    accumulatedPeakGR = 0.0f;
    accumulatedAvgGR = 0.0f;
    accumulatedClipCount = 0;
}

void LevelMeterData::publishFrame()
{
    LimiterMeterFrame* dest = meterBuffer.getWritePointer();
    *dest = currentFrame;
    meterBuffer.finishWrite();
}

float LevelMeterData::computePeakDB (const float* data, int numSamples) const
{
    float maxAbs = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        float absVal = std::fabs (data[i]);
        if (absVal > maxAbs)
            maxAbs = absVal;
    }

    if (maxAbs < 1e-10f)
        return Constants::kMeterMinDB;

    float db = 20.0f * std::log10 (maxAbs);
    return std::max (db, Constants::kMeterMinDB);
}

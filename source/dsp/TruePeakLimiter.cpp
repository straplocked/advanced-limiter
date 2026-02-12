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

#include "TruePeakLimiter.h"
#include <cmath>
#include <algorithm>

void TruePeakLimiter::prepare (double sampleRate)
{
    // Init detectors
    for (auto& d : detectors)
        d.prepare (sampleRate);

    // Allocate delay lines for ~0.2ms lookahead
    lookaheadSamples = std::max (1, (int) (0.0002 * sampleRate));

    int delaySize = lookaheadSamples + 1;
    for (int ch = 0; ch < 2; ++ch)
        delayLines[ch].resize ((size_t) delaySize, 0.0f);

    reset();
}

void TruePeakLimiter::reset()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        std::fill (delayLines[ch].begin(), delayLines[ch].end(), 0.0f);
        writePos[ch] = 0;
        sampleCounter[ch] = 0;
        minGainDeque[ch].clear();
        detectors[ch].reset();
    }
}

void TruePeakLimiter::setCeiling (float newCeilingDB)
{
    ceilingDB = newCeilingDB;
    ceiling = std::pow (10.0f, ceilingDB / 20.0f);
}

float TruePeakLimiter::processSample (float input, int channel)
{
    int ch = std::clamp (channel, 0, 1);
    int delaySize = (int) delayLines[ch].size();

    if (delaySize <= 0)
        return input;

    // Step 1: Write input to delay line
    delayLines[ch][(size_t) writePos[ch]] = input;

    // Step 2: Detect true peak
    float truePeak = detectors[ch].processSample (input);

    // Step 3: Compute needed gain
    float neededGain = 1.0f;
    if (truePeak > ceiling && truePeak > 1e-10f)
        neededGain = ceiling / truePeak;

    // Step 4: Use sliding window minimum over lookahead for smooth gain
    // Remove from back any entries that are less restrictive (larger gain)
    while (! minGainDeque[ch].empty() && minGainDeque[ch].back().gain >= neededGain)
        minGainDeque[ch].pop_back();

    // Push new entry
    minGainDeque[ch].push_back ({ neededGain, sampleCounter[ch] });

    // Remove expired entries from front
    while (! minGainDeque[ch].empty()
           && (sampleCounter[ch] - minGainDeque[ch].front().pos) >= lookaheadSamples)
        minGainDeque[ch].pop_front();

    // Front = minimum gain in the window
    float appliedGain = minGainDeque[ch].empty() ? 1.0f : minGainDeque[ch].front().gain;

    // Step 5: Read delayed sample
    int readPos = writePos[ch] - lookaheadSamples;
    if (readPos < 0)
        readPos += delaySize;

    float delayed = delayLines[ch][(size_t) readPos];

    // Step 6: Apply gain
    float output = delayed * appliedGain;

    // Step 7: Hard clip at ceiling as final safety
    output = std::clamp (output, -ceiling, ceiling);

    // Advance write position and sample counter
    writePos[ch] = (writePos[ch] + 1) % delaySize;
    sampleCounter[ch]++;

    return output;
}

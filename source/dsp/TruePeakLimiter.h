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

#pragma once

#include "TruePeakDetector.h"
#include <vector>
#include <deque>

class TruePeakLimiter
{
public:
    TruePeakLimiter() = default;

    void prepare (double sampleRate);
    void reset();
    void setCeiling (float ceilingDB);

    // Process one sample, returns corrected sample
    float processSample (float input, int channel);

    int getLatencySamples() const { return lookaheadSamples; }

private:
    TruePeakDetector detectors[2];
    std::vector<float> delayLines[2];
    int writePos[2] = { 0, 0 };
    int lookaheadSamples = 0;
    float ceiling = 1.0f;    // linear
    float ceilingDB = -0.3f;

    // Sliding window minimum for gain correction
    struct Entry
    {
        float gain;
        int pos;
    };
    std::deque<Entry> minGainDeque[2];
    int sampleCounter[2] = { 0, 0 };
};

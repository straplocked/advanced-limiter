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

#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>

/**
 * @file LoudnessRangeMeter.h
 * @brief EBU R128 Loudness Range (LRA) meter.
 *
 * Computes LRA from accumulated short-term loudness measurements using
 * absolute gating (-70 LUFS), relative gating (-20 dB below average),
 * and the difference between the 10th and 95th percentiles of the
 * gated distribution.
 */
class LoudnessRangeMeter
{
public:
    void prepare (double sampleRate)
    {
        sr = sampleRate;
        blockSize = (int) (0.4 * sr);  // 400ms blocks
        hopSize = (int) (0.1 * sr);    // 100ms hop (75% overlap)
        hopCounter = 0;
        shortTermBlocks.clear();
    }

    void reset()
    {
        shortTermBlocks.clear();
        hopCounter = 0;
        lra = 0.0f;
    }

    // Call this with each short-term loudness value from LoudnessMeter
    // (called at block rate, typically every buffer)
    void pushShortTermLUFS (double stLUFS)
    {
        hopCounter++;
        // Collect a short-term measurement every ~1 second
        // Short-term LUFS from the loudness meter is already 3s integrated
        if (hopCounter >= (int) (sr / 4096.0)) // approximate block interval
        {
            hopCounter = 0;
            if (stLUFS > -100.0)
                shortTermBlocks.push_back (stLUFS);
            computeLRA();
        }
    }

    float getLRA() const { return lra; }

private:
    void computeLRA()
    {
        if (shortTermBlocks.size() < 2)
        {
            lra = 0.0f;
            return;
        }

        // Step 1: Absolute gate at -70 LUFS
        std::vector<double> ungated;
        for (double v : shortTermBlocks)
            if (v > -70.0)
                ungated.push_back (v);

        if (ungated.size() < 2)
        {
            lra = 0.0f;
            return;
        }

        // Step 2: Relative gate at -20 dB below average
        double sum = std::accumulate (ungated.begin(), ungated.end(), 0.0);
        double avg = sum / (double) ungated.size();
        double relThresh = avg - 20.0;

        std::vector<double> gated;
        for (double v : ungated)
            if (v > relThresh)
                gated.push_back (v);

        if (gated.size() < 2)
        {
            lra = 0.0f;
            return;
        }

        // Step 3: Sort and compute 10th and 95th percentiles
        std::sort (gated.begin(), gated.end());
        int n = (int) gated.size();
        double low = gated[(int) (0.10 * (n - 1))];
        double high = gated[(int) (0.95 * (n - 1))];

        lra = (float) (high - low);
        lra = std::max (0.0f, lra);
    }

    double sr = 48000.0;
    int blockSize = 19200;
    int hopSize = 4800;
    int hopCounter = 0;
    std::vector<double> shortTermBlocks;
    float lra = 0.0f;
};

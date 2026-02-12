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

#include "TruePeakMeter.h"
#include <cmath>
#include <algorithm>

void TruePeakMeter::prepare (double sampleRate)
{
    for (auto& d : detectors)
        d.prepare (sampleRate);

    reset();
}

void TruePeakMeter::reset()
{
    for (auto& d : detectors)
        d.reset();

    for (auto& p : currentTruePeak)
        p.store (-100.0f, std::memory_order_relaxed);

    for (auto& p : peakHold)
        p.store (-100.0f, std::memory_order_relaxed);
}

void TruePeakMeter::processSample (float sample, int channel)
{
    if (channel < 0 || channel >= 2)
        return;

    float truePeakLinear = detectors[(size_t) channel].processSample (sample);

    // Convert to dB
    float truePeakDB = (truePeakLinear > 1e-10f)
        ? 20.0f * std::log10 (truePeakLinear)
        : -100.0f;

    // Update current true peak
    currentTruePeak[(size_t) channel].store (truePeakDB, std::memory_order_relaxed);

    // Update peak hold (max)
    float currentHold = peakHold[(size_t) channel].load (std::memory_order_relaxed);
    if (truePeakDB > currentHold)
        peakHold[(size_t) channel].store (truePeakDB, std::memory_order_relaxed);
}

void TruePeakMeter::processBlock (const float* const* channelData, int numChannels, int numSamples)
{
    int ch = std::min (numChannels, 2);

    for (int c = 0; c < ch; ++c)
    {
        for (int i = 0; i < numSamples; ++i)
            processSample (channelData[c][i], c);
    }
}

float TruePeakMeter::getTruePeakDB (int channel) const
{
    if (channel < 0 || channel >= 2)
        return -100.0f;

    return peakHold[(size_t) channel].load (std::memory_order_relaxed);
}

void TruePeakMeter::resetPeakHold()
{
    for (auto& p : peakHold)
        p.store (-100.0f, std::memory_order_relaxed);
}

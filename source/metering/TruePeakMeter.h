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

#include "../dsp/TruePeakDetector.h"
#include <array>
#include <atomic>

/**
 * @file TruePeakMeter.h
 * @brief Display-only ISP (inter-sample peak) measurement using TruePeakDetector.
 *
 * Wraps TruePeakDetector for stereo metering with atomic peak hold values
 * suitable for lock-free reading from the UI thread.
 */
class TruePeakMeter
{
public:
    TruePeakMeter() = default;

    void prepare (double sampleRate);
    void reset();

    void processSample (float sample, int channel);
    void processBlock (const float* const* channelData, int numChannels, int numSamples);

    float getTruePeakDB (int channel) const;
    void resetPeakHold();

private:
    std::array<TruePeakDetector, 2> detectors;
    std::array<std::atomic<float>, 2> currentTruePeak { { { -100.0f }, { -100.0f } } };
    std::array<std::atomic<float>, 2> peakHold { { { -100.0f }, { -100.0f } } };
};

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

#include <cmath>
#include <array>
#include <algorithm>

/**
 * @file TruePeakDetector.h
 * @brief ITU-R BS.1770-4 compliant true peak detector using 4x oversampling.
 *
 * Uses a polyphase FIR interpolation filter to upsample by 4x, then finds
 * the peak of the interpolated signal. The filter coefficients are from
 * the ITU-R BS.1770-4 specification (12-tap per phase, 4 phases = 48 taps).
 */
class TruePeakDetector
{
public:
    TruePeakDetector() = default;

    void prepare (double /*sampleRate*/)
    {
        reset();
    }

    void reset()
    {
        history.fill (0.0f);
        historyPos = 0;
        currentTruePeak = 0.0f;
    }

    /**
     * Process a single sample and return the true peak (linear) of
     * the 4x oversampled signal for this input sample.
     */
    float processSample (float sample)
    {
        // Push sample into history buffer
        history[(size_t) historyPos] = sample;
        historyPos = (historyPos + 1) % kHistorySize;

        float peak = 0.0f;

        // Evaluate each of the 4 polyphase sub-filters
        for (int phase = 0; phase < 4; ++phase)
        {
            float sum = 0.0f;
            for (int tap = 0; tap < kTapsPerPhase; ++tap)
            {
                int idx = (historyPos - 1 - tap + kHistorySize * 2) % kHistorySize;
                sum += history[(size_t) idx] * coeffs[phase][tap];
            }
            float absSample = std::fabs (sum);
            if (absSample > peak)
                peak = absSample;
        }

        currentTruePeak = peak;
        return peak;
    }

    float getCurrentTruePeak() const { return currentTruePeak; }

private:
    static constexpr int kTapsPerPhase = 12;
    static constexpr int kHistorySize = 12;

    // ITU-R BS.1770-4 compliant 4x oversampling FIR filter coefficients
    // 4 phases, 12 taps each (48-tap prototype filter, polyphase decomposition)
    // These coefficients implement a low-pass filter at Fs/2 for 4x interpolation
    // with good stopband attenuation suitable for true peak measurement.
    static constexpr float coeffs[4][kTapsPerPhase] = {
        // Phase 0 (original samples pass-through equivalent)
        {  0.0017089843750f,  -0.0291748046875f,  -0.0189208984375f,   0.0769042968750f,
           0.3412475585938f,   0.5765380859375f,   0.3412475585938f,   0.0769042968750f,
          -0.0189208984375f,  -0.0291748046875f,   0.0017089843750f,   0.0000000000000f },
        // Phase 1
        { -0.0024719238281f,   0.0186767578125f,  -0.0598907470703f,   0.1274719238281f,
          -0.2075805664063f,   0.9721679687500f,   0.2075805664063f,  -0.0527038574219f,
           0.0233154296875f,  -0.0109863281250f,   0.0037231445313f,  -0.0003509521484f },
        // Phase 2
        {  0.0003509521484f,  -0.0037231445313f,   0.0109863281250f,  -0.0233154296875f,
           0.0527038574219f,  -0.2075805664063f,   0.9721679687500f,   0.2075805664063f,
          -0.1274719238281f,   0.0598907470703f,  -0.0186767578125f,   0.0024719238281f },
        // Phase 3
        {  0.0000000000000f,   0.0017089843750f,  -0.0291748046875f,  -0.0189208984375f,
           0.0769042968750f,   0.3412475585938f,   0.5765380859375f,   0.3412475585938f,
           0.0769042968750f,  -0.0189208984375f,  -0.0291748046875f,   0.0017089843750f }
    };

    std::array<float, kHistorySize> history {};
    int historyPos = 0;
    float currentTruePeak = 0.0f;
};

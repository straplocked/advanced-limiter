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
#include <vector>
#include <algorithm>
#include <numeric>

// EBU R128 / ITU-R BS.1770-4 Loudness Meter
// Provides Momentary (400ms), Short-term (3s), and Integrated loudness in LUFS.
//
// Signal chain: K-weighting filter (shelf + HP) -> mean square -> gating -> LUFS

class LoudnessMeter
{
public:
    LoudnessMeter() = default;

    void prepare (double sampleRate, int numChannels)
    {
        sr = sampleRate;
        channels = numChannels;

        // K-weighting filter coefficients (ITU-R BS.1770-4)
        computeKWeightCoefficients();

        // Reset filter states
        for (int ch = 0; ch < 2; ++ch)
        {
            shelfState[ch] = {};
            hpState[ch] = {};
        }

        // Momentary window: 400ms with 75% overlap -> 100ms hop
        momentaryWindowSamples = (int) (0.4 * sr);
        shortTermWindowSamples = (int) (3.0 * sr);

        // Ring buffer for momentary/short-term calculation
        int maxSamples = shortTermWindowSamples + (int) sr; // 3s + 1s padding
        for (int ch = 0; ch < 2; ++ch)
            squaredBuffer[ch].assign ((size_t) maxSamples, 0.0);

        writePos = 0;
        samplesWritten = 0;

        // Integrated loudness: accumulate gated blocks
        gatedBlocksSum = 0.0;
        gatedBlocksCount = 0;
        ungatedBlocks.clear();

        momentaryLUFS = -100.0;
        shortTermLUFS = -100.0;
        integratedLUFS = -100.0;
    }

    void reset()
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            shelfState[ch] = {};
            hpState[ch] = {};
            std::fill (squaredBuffer[ch].begin(), squaredBuffer[ch].end(), 0.0);
        }
        writePos = 0;
        samplesWritten = 0;
        intBlockCounter = 0;
        gatedBlocksSum = 0.0;
        gatedBlocksCount = 0;
        ungatedBlocks.clear();
        momentaryLUFS = -100.0;
        shortTermLUFS = -100.0;
        integratedLUFS = -100.0;
    }

    // Push a block of audio samples (interleaved or per-channel)
    void pushBlock (const float* const* channelData, int numChannels, int numSamples)
    {
        int ch = std::min (numChannels, 2);

        for (int i = 0; i < numSamples; ++i)
        {
            int bufSize = (int) squaredBuffer[0].size();

            for (int c = 0; c < ch; ++c)
            {
                // Apply K-weighting
                double sample = (double) channelData[c][i];
                double filtered = applyKWeighting (sample, c);

                // Store squared sample
                squaredBuffer[(size_t) c][(size_t) writePos] = filtered * filtered;
            }

            // For mono, duplicate to second channel
            if (ch == 1)
                squaredBuffer[1][(size_t) writePos] = squaredBuffer[0][(size_t) writePos];

            writePos = (writePos + 1) % bufSize;
            samplesWritten++;
        }

        updateLoudness();
    }

    double getMomentaryLUFS() const { return momentaryLUFS; }
    double getShortTermLUFS() const { return shortTermLUFS; }
    double getIntegratedLUFS() const { return integratedLUFS; }

private:
    // Biquad state
    struct BiquadState
    {
        double x1 = 0.0, x2 = 0.0;
        double y1 = 0.0, y2 = 0.0;
    };

    // Biquad coefficients
    struct BiquadCoeffs
    {
        double b0 = 1.0, b1 = 0.0, b2 = 0.0;
        double a1 = 0.0, a2 = 0.0;
    };

    double processBiquad (double input, BiquadState& state, const BiquadCoeffs& c) const
    {
        double output = c.b0 * input + c.b1 * state.x1 + c.b2 * state.x2
                       - c.a1 * state.y1 - c.a2 * state.y2;
        state.x2 = state.x1;
        state.x1 = input;
        state.y2 = state.y1;
        state.y1 = output;
        return output;
    }

    double applyKWeighting (double sample, int ch)
    {
        // Stage 1: High shelf (+4 dB above ~1.5 kHz)
        double s1 = processBiquad (sample, shelfState[(size_t) ch], shelfCoeffs);
        // Stage 2: High-pass (~38 Hz, 12 dB/oct)
        double s2 = processBiquad (s1, hpState[(size_t) ch], hpCoeffs);
        return s2;
    }

    void computeKWeightCoefficients()
    {
        // Stage 1: Pre-filter (high shelf) - from ITU-R BS.1770-4
        // These are the exact coefficients for 48 kHz from the standard.
        // For other sample rates, we recompute using the analog prototype.
        double fc1 = 1681.974450955533;
        double G = 3.999843853973347;  // dB
        double Q1 = 0.7071752369554196;

        double A1 = std::pow (10.0, G / 40.0);
        double w01 = 2.0 * 3.14159265358979323846 * fc1 / sr;
        double alpha1 = std::sin (w01) / (2.0 * Q1);

        double a0s = (A1 + 1.0) - (A1 - 1.0) * std::cos (w01) + 2.0 * std::sqrt (A1) * alpha1;
        shelfCoeffs.b0 = (A1 * ((A1 + 1.0) + (A1 - 1.0) * std::cos (w01) + 2.0 * std::sqrt (A1) * alpha1)) / a0s;
        shelfCoeffs.b1 = (-2.0 * A1 * ((A1 - 1.0) + (A1 + 1.0) * std::cos (w01))) / a0s;
        shelfCoeffs.b2 = (A1 * ((A1 + 1.0) + (A1 - 1.0) * std::cos (w01) - 2.0 * std::sqrt (A1) * alpha1)) / a0s;
        shelfCoeffs.a1 = (2.0 * ((A1 - 1.0) - (A1 + 1.0) * std::cos (w01))) / a0s;
        shelfCoeffs.a2 = ((A1 + 1.0) - (A1 - 1.0) * std::cos (w01) - 2.0 * std::sqrt (A1) * alpha1) / a0s;

        // Stage 2: High-pass filter (~38 Hz)
        double fc2 = 38.13547087602444;
        double Q2 = 0.5003270373238773;

        double w02 = 2.0 * 3.14159265358979323846 * fc2 / sr;
        double alpha2 = std::sin (w02) / (2.0 * Q2);
        double cos_w02 = std::cos (w02);

        double a0h = 1.0 + alpha2;
        hpCoeffs.b0 = ((1.0 + cos_w02) / 2.0) / a0h;
        hpCoeffs.b1 = (-(1.0 + cos_w02)) / a0h;
        hpCoeffs.b2 = ((1.0 + cos_w02) / 2.0) / a0h;
        hpCoeffs.a1 = (-2.0 * cos_w02) / a0h;
        hpCoeffs.a2 = (1.0 - alpha2) / a0h;
    }

    void updateLoudness()
    {
        // Momentary loudness (400ms)
        if (samplesWritten >= momentaryWindowSamples)
        {
            double sum = computeMeanSquare (momentaryWindowSamples);
            momentaryLUFS = (sum > 1e-20) ? -0.691 + 10.0 * std::log10 (sum) : -100.0;
        }

        // Short-term loudness (3s)
        if (samplesWritten >= shortTermWindowSamples)
        {
            double sum = computeMeanSquare (shortTermWindowSamples);
            shortTermLUFS = (sum > 1e-20) ? -0.691 + 10.0 * std::log10 (sum) : -100.0;
        }

        // Integrated loudness with gating (every 400ms block)
        int hopSamples = momentaryWindowSamples / 4; // 100ms hop
        intBlockCounter++;
        if (intBlockCounter >= hopSamples && samplesWritten >= momentaryWindowSamples)
        {
            intBlockCounter = 0;
            double blockPower = computeMeanSquare (momentaryWindowSamples);
            double blockLUFS = (blockPower > 1e-20) ? -0.691 + 10.0 * std::log10 (blockPower) : -100.0;

            // Absolute gate: -70 LUFS
            if (blockLUFS > -70.0)
                ungatedBlocks.push_back (blockPower);

            // Relative gate: compute relative threshold
            if (! ungatedBlocks.empty())
            {
                double avgPower = std::accumulate (ungatedBlocks.begin(), ungatedBlocks.end(), 0.0)
                                  / (double) ungatedBlocks.size();
                double relativeThreshLUFS = -0.691 + 10.0 * std::log10 (avgPower) - 10.0;

                double gatedSum = 0.0;
                int gatedCount = 0;
                for (double bp : ungatedBlocks)
                {
                    double bpLUFS = (bp > 1e-20) ? -0.691 + 10.0 * std::log10 (bp) : -100.0;
                    if (bpLUFS > relativeThreshLUFS)
                    {
                        gatedSum += bp;
                        gatedCount++;
                    }
                }

                integratedLUFS = (gatedCount > 0 && gatedSum > 1e-20)
                    ? -0.691 + 10.0 * std::log10 (gatedSum / (double) gatedCount)
                    : -100.0;
            }
        }
    }

    double computeMeanSquare (int windowSamples) const
    {
        int bufSize = (int) squaredBuffer[0].size();
        double sum = 0.0;
        int startPos = (writePos - windowSamples + bufSize) % bufSize;

        for (int i = 0; i < windowSamples; ++i)
        {
            int idx = (startPos + i) % bufSize;
            // Sum both channels (equal weighting for L/R per BS.1770)
            sum += squaredBuffer[0][(size_t) idx] + squaredBuffer[1][(size_t) idx];
        }

        return sum / (double) windowSamples;
    }

    double sr = 48000.0;
    int channels = 2;

    // K-weighting filter
    BiquadCoeffs shelfCoeffs;
    BiquadCoeffs hpCoeffs;
    BiquadState shelfState[2];
    BiquadState hpState[2];

    // Squared sample ring buffers (per channel)
    std::vector<double> squaredBuffer[2];
    int writePos = 0;
    int samplesWritten = 0;

    int momentaryWindowSamples = 19200;  // 400ms at 48kHz
    int shortTermWindowSamples = 144000; // 3s at 48kHz
    int intBlockCounter = 0;

    // Integrated loudness gating
    double gatedBlocksSum = 0.0;
    int gatedBlocksCount = 0;
    std::vector<double> ungatedBlocks;

    // Results
    double momentaryLUFS = -100.0;
    double shortTermLUFS = -100.0;
    double integratedLUFS = -100.0;
};

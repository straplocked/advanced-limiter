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

#include "LimiterCore.h"
#include "../utils/MathUtils.h"
#include <cmath>
#include <algorithm>

void LimiterCore::prepare (double sampleRate, int /*maxBlockSize*/)
{
    currentSampleRate = sampleRate;

    // Compute max lookahead samples (10ms at current sample rate)
    int maxLookaheadSamples = (int) (0.010 * sampleRate) + 1;

    for (auto& ts : transientStages)
        ts.prepare (sampleRate, maxLookaheadSamples);

    for (auto& rs : releaseStages)
        rs.prepare (sampleRate);

    reset();
}

void LimiterCore::reset()
{
    for (auto& ts : transientStages)
        ts.reset();

    for (auto& rs : releaseStages)
        rs.reset();
}

void LimiterCore::updateParameters (float lookaheadMs, float attackMs, float releaseMs,
                                     float transientLink, float releaseLink,
                                     float ceilingDB, LimiterStyle style)
{
    // Get style behavior
    styleBehavior = getStyleBehavior (style);

    // Scale parameters by style multipliers
    float scaledLookahead = lookaheadMs * styleBehavior.lookaheadScale;
    float scaledAttack = attackMs * styleBehavior.attackScale;
    float scaledRelease = releaseMs * styleBehavior.releaseScale;

    // Compute lookahead in samples
    lookaheadSamplesInt = std::max (1, (int) (scaledLookahead * 0.001f * (float) currentSampleRate));

    // Set transient stages
    for (auto& ts : transientStages)
    {
        ts.setLookaheadSamples (lookaheadSamplesInt);
        ts.setTransientPreserve (styleBehavior.transientPreserve);
    }

    // Set release stages
    for (auto& rs : releaseStages)
    {
        rs.setAttackMs (scaledAttack);
        rs.setReleaseMs (scaledRelease);
        rs.setHoldMs (styleBehavior.holdMs);
        rs.setProgramDependentRelease (styleBehavior.programDepRelease);
    }

    // Store ceiling and link amounts (convert from 0-100% to 0-1)
    ceiling = ceilingDB;
    transientLinkAmount = transientLink * 0.01f;
    releaseLinkAmount = releaseLink * 0.01f;
}

void LimiterCore::process (float** channelData, int numChannels, int numSamples,
                            float& grOutL, float& grOutR)
{
    int chCount = std::min (numChannels, 2);
    float peakGRL = 0.0f;
    float peakGRR = 0.0f;

    for (int i = 0; i < numSamples; ++i)
    {
        // Step a: Feed each channel's sample to its transient stage
        float transientGR[2] = { 0.0f, 0.0f };
        for (int ch = 0; ch < chCount; ++ch)
            transientGR[ch] = transientStages[(size_t) ch].processSample (channelData[ch][i], ceiling);

        // For mono, duplicate
        if (chCount == 1)
            transientGR[1] = transientGR[0];

        // Step c: Channel link for transient GR
        // min() picks the most aggressive (most negative) GR
        float linkedTransientGR = std::min (transientGR[0], transientGR[1]);
        float smoothedTransientGR[2];
        for (int ch = 0; ch < chCount; ++ch)
        {
            smoothedTransientGR[ch] = transientLinkAmount * linkedTransientGR
                                    + (1.0f - transientLinkAmount) * transientGR[ch];
        }

        // Step d: Feed linked GR to release stage per channel
        float releaseGR[2] = { 0.0f, 0.0f };
        for (int ch = 0; ch < chCount; ++ch)
            releaseGR[ch] = releaseStages[(size_t) ch].processSample (smoothedTransientGR[ch]);

        // For mono, duplicate
        if (chCount == 1)
            releaseGR[1] = releaseGR[0];

        // Step f: Channel link for release GR
        float linkedReleaseGR = std::min (releaseGR[0], releaseGR[1]);
        float finalGR[2];
        for (int ch = 0; ch < chCount; ++ch)
        {
            finalGR[ch] = releaseLinkAmount * linkedReleaseGR
                        + (1.0f - releaseLinkAmount) * releaseGR[ch];
        }

        // Step g: Read delayed sample from transient stage
        // Step h: Apply combined GR: output = delayed * dbToGain(finalGR)
        for (int ch = 0; ch < chCount; ++ch)
        {
            float delayed = transientStages[(size_t) ch].readDelayed();
            float gain = MathUtils::dbToGain (finalGR[ch]);
            float output = delayed * gain;

            // Step i: Soft clip with tanh if style has clipAmount > 0
            if (styleBehavior.clipAmount > 0.0f)
            {
                float ceilLinear = MathUtils::dbToGain (ceiling);
                // Normalize to ceiling, apply tanh, scale back
                float normalized = output / ceilLinear;
                float clipped = std::tanh (normalized);
                // Blend between clean and clipped
                output = ceilLinear * (styleBehavior.clipAmount * clipped
                                     + (1.0f - styleBehavior.clipAmount) * normalized);
            }

            // Step j: Write back to channelData
            channelData[ch][i] = output;
        }

        // Track peak GR for metering
        peakGRL = std::min (peakGRL, finalGR[0]);
        peakGRR = std::min (peakGRR, finalGR[chCount > 1 ? 1 : 0]);
    }

    // Store peak GR for metering output
    grOutL = peakGRL;
    grOutR = peakGRR;
}

int LimiterCore::getLatencySamples() const
{
    return lookaheadSamplesInt;
}

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

#include "ReleaseStage.h"
#include <cmath>
#include <algorithm>

void ReleaseStage::prepare (double sampleRate)
{
    sr = sampleRate;
    reset();
}

void ReleaseStage::reset()
{
    currentGR = 0.0f;
    holdCounter = 0;
    grAccumulator = 0.0f;
    grDurationSamples = 0;
    adaptiveReleaseCoeff = 0.0f;
}

void ReleaseStage::setAttackMs (float ms)
{
    // Prevent division by zero for very small attack times
    float timeSec = std::max (ms * 0.001f, 1e-6f);
    attackCoeff = std::exp (-1.0f / (float (sr) * timeSec));
}

void ReleaseStage::setReleaseMs (float ms)
{
    float timeSec = std::max (ms * 0.001f, 1e-6f);
    releaseCoeff = std::exp (-1.0f / (float (sr) * timeSec));
}

void ReleaseStage::setHoldMs (float ms)
{
    holdSamples = (int) (ms * 0.001f * (float) sr);
}

void ReleaseStage::setProgramDependentRelease (bool enabled)
{
    programDepRelease = enabled;
}

float ReleaseStage::processSample (float targetGRdB)
{
    if (targetGRdB < currentGR)
    {
        // Need more reduction (attack phase)
        holdCounter = holdSamples;
        currentGR += (1.0f - attackCoeff) * (targetGRdB - currentGR);

        // Accumulate statistics for program-dependent release
        if (programDepRelease)
        {
            grDurationSamples++;
            grAccumulator += std::abs (targetGRdB);
        }
    }
    else if (holdCounter > 0)
    {
        // Hold phase: maintain current GR level
        holdCounter--;
    }
    else
    {
        // Release phase
        float relCoeff = releaseCoeff;

        if (programDepRelease && grDurationSamples > 0)
        {
            // Compute average GR depth during the limiting event
            float avgGRDepth = grAccumulator / (float) grDurationSamples;

            // Compute duration in milliseconds
            float durationMs = (float) grDurationSamples / (float) sr * 1000.0f;

            // Adaptive release: deeper and longer events get slower release
            // Scale factor ranges from 0.5 (fast) to 3.0 (slow)
            float depthFactor = std::clamp (avgGRDepth / 12.0f, 0.0f, 1.0f);
            float durationFactor = std::clamp (durationMs / 200.0f, 0.0f, 1.0f);
            float adaptiveScale = 0.5f + 2.5f * (depthFactor * 0.6f + durationFactor * 0.4f);

            // Compute adaptive release coefficient
            float baseTimeConst = -1.0f / std::log (releaseCoeff + 1e-30f);
            float adaptiveTimeConst = baseTimeConst * adaptiveScale;
            adaptiveReleaseCoeff = std::exp (-1.0f / std::max (adaptiveTimeConst, 1.0f));

            // Blend adaptive with base release: 70% adaptive, 30% base
            relCoeff = 0.3f * releaseCoeff + 0.7f * adaptiveReleaseCoeff;
        }

        currentGR += (1.0f - relCoeff) * (targetGRdB - currentGR);

        // When GR is close to zero, snap to zero and reset accumulators
        if (std::abs (currentGR) < 0.01f)
        {
            currentGR = 0.0f;
            grAccumulator = 0.0f;
            grDurationSamples = 0;
        }
    }

    return currentGR;
}

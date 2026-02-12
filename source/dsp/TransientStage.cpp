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

#include "TransientStage.h"
#include <cmath>
#include <algorithm>

void TransientStage::prepare (double /*sampleRate*/, int maxLookaheadSamples)
{
    delaySize = maxLookaheadSamples + 1;
    delayLine.resize ((size_t) delaySize, 0.0f);
    reset();
}

void TransientStage::reset()
{
    std::fill (delayLine.begin(), delayLine.end(), 0.0f);
    writePos = 0;
    sampleCounter = 0;
    minDeque.clear();
}

void TransientStage::setLookaheadSamples (int samples)
{
    lookaheadSamples = std::min (samples, delaySize - 1);
}

void TransientStage::setTransientPreserve (float amount)
{
    transientPreserve = std::clamp (amount, 0.0f, 1.0f);
}

float TransientStage::processSample (float inputSample, float ceilingDB)
{
    // Step 1: Write input to delay line at writePos
    if (delaySize > 0)
        delayLine[(size_t) writePos] = inputSample;

    // Step 2: Compute peak level in dB
    float absInput = std::abs (inputSample);
    float peakDB;
    if (absInput < 1e-10f)
        peakDB = -200.0f; // effectively -inf
    else
        peakDB = 20.0f * std::log10 (absInput);

    // Step 3: Compute target gain reduction (negative or zero)
    float targetGR = std::min (0.0f, ceilingDB - peakDB);

    // Step 4: Apply transient preserve - scale GR to allow brief overshoot
    // When transientPreserve is 1.0, no GR is applied (full overshoot allowed)
    // When transientPreserve is 0.0, full GR is applied
    targetGR *= (1.0f - transientPreserve);

    // Step 5: Remove from back of deque any entries less aggressive (> targetGR)
    // We want the minimum (most negative) GR, so remove anything greater
    while (! minDeque.empty() && minDeque.back().grDB > targetGR)
        minDeque.pop_back();

    // Step 6: Push new entry to back
    minDeque.push_back ({ targetGR, sampleCounter });

    // Step 7: Remove expired entries from front (older than lookaheadSamples)
    while (! minDeque.empty()
           && (sampleCounter - minDeque.front().position) >= lookaheadSamples)
        minDeque.pop_front();

    // Step 8: Front of deque = most aggressive GR needed in the window
    float resultGR = minDeque.empty() ? 0.0f : minDeque.front().grDB;

    // Step 9: Advance write position and sample counter
    writePos = (writePos + 1) % delaySize;
    sampleCounter++;

    // Step 10: Return the gain reduction value
    return resultGR;
}

float TransientStage::readDelayed() const
{
    if (delaySize <= 0)
        return 0.0f;

    // Read from (writePos - lookaheadSamples) wrapped
    int readPos = writePos - lookaheadSamples;
    if (readPos < 0)
        readPos += delaySize;

    return delayLine[(size_t) readPos];
}

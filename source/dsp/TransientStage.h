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
#include <deque>
#include <cstdint>

class TransientStage
{
public:
    TransientStage() = default;

    void prepare (double sampleRate, int maxLookaheadSamples);
    void reset();

    void setLookaheadSamples (int samples);
    void setTransientPreserve (float amount); // 0-1

    // Process one sample: returns gain reduction in dB (negative or zero)
    float processSample (float inputSample, float ceilingDB);

    // Read from delay line at current position
    float readDelayed() const;

private:
    std::vector<float> delayLine;
    int delaySize = 0;
    int writePos = 0;
    int lookaheadSamples = 0;
    float transientPreserve = 0.0f;

    // Monotone deque for sliding-window minimum
    struct DequeEntry
    {
        float grDB;
        int64_t position;
    };
    std::deque<DequeEntry> minDeque;
    int64_t sampleCounter = 0;
};

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

#include "TransientStage.h"
#include "ReleaseStage.h"
#include "LimiterStyle.h"
#include <array>

class LimiterCore
{
public:
    LimiterCore() = default;

    void prepare (double sampleRate, int maxBlockSize);
    void reset();

    void updateParameters (float lookaheadMs, float attackMs, float releaseMs,
                           float transientLink, float releaseLink,
                           float ceilingDB, LimiterStyle style);

    // Process a block of audio in-place. Returns per-channel GR in dB.
    void process (float** channelData, int numChannels, int numSamples,
                  float& grOutL, float& grOutR);

    int getLatencySamples() const;

private:
    std::array<TransientStage, 2> transientStages;
    std::array<ReleaseStage, 2> releaseStages;

    double currentSampleRate = 44100.0;
    float ceiling = -0.3f;
    float transientLinkAmount = 1.0f;
    float releaseLinkAmount = 1.0f;
    LimiterStyleBehavior styleBehavior;
    int lookaheadSamplesInt = 0;
};

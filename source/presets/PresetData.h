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

#include <juce_core/juce_core.h>

struct LimiterPresetData
{
    float inputGain = 0.0f;
    float outputLevel = -0.3f;
    int   style = 4;              // Modern
    float lookaheadMs = 2.0f;
    float attackMs = 0.1f;
    float releaseMs = 100.0f;
    float transientLink = 100.0f;
    float releaseLink = 100.0f;
    bool  truePeak = true;
};

struct Preset
{
    juce::String        name;
    juce::String        category;
    LimiterPresetData   data;
    bool                isFactory = true;
    bool                isFavorite = false;
    juce::File          sourceFile;
};

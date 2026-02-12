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

#include "FactoryPresets.h"

// LimiterPresetData field order:
// { inputGain, outputLevel, style, lookaheadMs, attackMs, releaseMs,
//   transientLink, releaseLink, truePeak }
//
// Style indices: 0=Transparent, 1=Punchy, 2=Dynamic, 3=Allround,
//                4=Modern, 5=Aggressive, 6=Bus, 7=Safe

namespace FactoryPresets
{

std::vector<Preset> getMasteringPresets()
{
    std::vector<Preset> presets;

    presets.push_back ({ "Mastering Transparent", "Mastering",
        { 0.0f, -0.3f, 0, 2.0f, 0.1f, 150.0f, 100.0f, 100.0f, true } });

    presets.push_back ({ "Mastering Loud", "Mastering",
        { 0.0f, -0.1f, 4, 1.0f, 0.05f, 80.0f, 100.0f, 100.0f, true } });

    presets.push_back ({ "Mastering Safe", "Mastering",
        { 0.0f, -0.5f, 7, 3.0f, 0.2f, 200.0f, 100.0f, 100.0f, true } });

    presets.push_back ({ "Mastering Punchy", "Mastering",
        { 0.0f, -0.3f, 1, 1.5f, 0.08f, 100.0f, 100.0f, 80.0f, true } });

    return presets;
}

std::vector<Preset> getMixingPresets()
{
    std::vector<Preset> presets;

    presets.push_back ({ "Mix Bus Glue", "Mixing",
        { 0.0f, -0.5f, 6, 3.0f, 0.5f, 200.0f, 100.0f, 100.0f, true } });

    presets.push_back ({ "Vocal Limiter", "Mixing",
        { 0.0f, -1.0f, 3, 2.0f, 0.2f, 150.0f, 100.0f, 100.0f, true } });

    presets.push_back ({ "Drum Limiter", "Mixing",
        { 0.0f, -0.3f, 5, 0.5f, 0.02f, 50.0f, 50.0f, 50.0f, true } });

    presets.push_back ({ "Bass Limiter", "Mixing",
        { 0.0f, -0.5f, 2, 2.0f, 0.1f, 120.0f, 100.0f, 100.0f, true } });

    return presets;
}

std::vector<Preset> getBroadcastPresets()
{
    std::vector<Preset> presets;

    presets.push_back ({ "EBU R128", "Broadcast",
        { 0.0f, -1.0f, 0, 2.0f, 0.1f, 200.0f, 100.0f, 100.0f, true } });

    presets.push_back ({ "Podcast", "Broadcast",
        { 0.0f, -1.0f, 3, 2.0f, 0.15f, 150.0f, 100.0f, 100.0f, true } });

    presets.push_back ({ "Streaming -14 LUFS", "Broadcast",
        { 0.0f, -1.0f, 4, 1.5f, 0.1f, 100.0f, 100.0f, 100.0f, true } });

    presets.push_back ({ "Film -23 LUFS", "Broadcast",
        { 0.0f, -1.0f, 7, 3.0f, 0.2f, 250.0f, 100.0f, 100.0f, true } });

    return presets;
}

std::vector<Preset> getCreativePresets()
{
    std::vector<Preset> presets;

    presets.push_back ({ "Brick Wall", "Creative",
        { 0.0f, 0.0f, 5, 0.1f, 0.01f, 30.0f, 100.0f, 100.0f, false } });

    presets.push_back ({ "Pumping", "Creative",
        { 0.0f, -0.3f, 1, 0.5f, 0.05f, 30.0f, 100.0f, 50.0f, true } });

    presets.push_back ({ "Saturator", "Creative",
        { 6.0f, -0.3f, 5, 0.3f, 0.02f, 40.0f, 100.0f, 100.0f, false } });

    presets.push_back ({ "Soft Clip", "Creative",
        { 3.0f, -0.5f, 4, 1.0f, 0.1f, 80.0f, 100.0f, 100.0f, true } });

    return presets;
}

std::vector<Preset> getAll()
{
    auto all = getMasteringPresets();
    auto mixing = getMixingPresets();
    auto broadcast = getBroadcastPresets();
    auto creative = getCreativePresets();

    all.insert (all.end(), mixing.begin(), mixing.end());
    all.insert (all.end(), broadcast.begin(), broadcast.end());
    all.insert (all.end(), creative.begin(), creative.end());
    return all;
}

} // namespace FactoryPresets

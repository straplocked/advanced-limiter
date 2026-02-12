/*
 * Advanced Limiter - Open-source lookahead limiter plugin
 * Copyright (C) 2024-2026 Advanced Limiter contributors
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#pragma once

#include <array>

enum class LimiterStyle
{
    Transparent = 0,
    Punchy,
    Dynamic,
    Allround,
    Modern,
    Aggressive,
    Bus,
    Safe
};

struct LimiterStyleBehavior
{
    float lookaheadScale;
    float attackScale;
    float releaseScale;
    float transientPreserve; // 0-1
    bool programDepRelease;
    float holdMs;
    float clipAmount; // 0-1
};

inline LimiterStyleBehavior getStyleBehavior (LimiterStyle style)
{
    // Table: {lookahead, attack, release, transient, progDep, hold, clip}
    static constexpr std::array<LimiterStyleBehavior, 8> table = {{
        { 1.0f, 1.0f, 1.0f, 0.3f, false, 0.0f, 0.0f  },  // Transparent
        { 0.5f, 0.3f, 0.7f, 0.8f, false, 0.5f, 0.1f  },  // Punchy
        { 1.0f, 0.8f, 1.2f, 0.5f, true,  0.0f, 0.0f  },  // Dynamic
        { 1.2f, 1.0f, 1.0f, 0.4f, true,  0.3f, 0.05f },  // Allround
        { 0.8f, 0.5f, 0.8f, 0.2f, false, 0.0f, 0.15f },  // Modern
        { 0.4f, 0.2f, 0.5f, 0.1f, false, 0.0f, 0.3f  },  // Aggressive
        { 1.5f, 1.5f, 2.0f, 0.6f, true,  1.0f, 0.0f  },  // Bus
        { 2.0f, 2.0f, 1.5f, 0.5f, false, 0.5f, 0.0f  },  // Safe
    }};
    return table[(size_t) style];
}

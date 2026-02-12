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

#include <juce_core/juce_core.h>

namespace ParamIDs
{
    constexpr const char* kInputGain     = "input_gain";
    constexpr const char* kOutputLevel   = "output_level";
    constexpr const char* kLimiterStyle  = "limiter_style";
    constexpr const char* kLookahead     = "lookahead";
    constexpr const char* kAttack        = "attack";
    constexpr const char* kRelease       = "release";
    constexpr const char* kTransientLink = "transient_link";
    constexpr const char* kReleaseLink   = "release_link";
    constexpr const char* kTruePeak      = "true_peak";
    constexpr const char* kDCFilter      = "dc_filter";
    constexpr const char* kDitherOn      = "dither_on";
    constexpr const char* kDitherBits    = "dither_bits";
    constexpr const char* kNoiseShape    = "noise_shape";
    constexpr const char* kOversample    = "oversample";
    constexpr const char* kSCExternal    = "sc_external";
    constexpr const char* kUnityGain     = "unity_gain";
    constexpr const char* kAudition      = "audition";
    constexpr const char* kDelta         = "delta";
    constexpr const char* kDisplayMode   = "display_mode";
    constexpr const char* kMeterScale    = "meter_scale";
    constexpr const char* kLoudnessTarget = "loudness_target";
    constexpr const char* kLoudnessTime  = "loudness_time";
    constexpr const char* kBypass        = "bypass";

    inline juce::StringArray getStyleNames()
    {
        return { "Transparent", "Punchy", "Dynamic", "Allround", "Modern", "Aggressive", "Bus", "Safe" };
    }

    inline juce::StringArray getOversamplingNames()
    {
        return { "1x", "2x", "4x", "8x", "16x", "32x" };
    }

    inline juce::StringArray getDitherBitNames()
    {
        return { "16 bit", "18 bit", "20 bit", "22 bit", "24 bit" };
    }

    inline juce::StringArray getNoiseShapeNames()
    {
        return { "Off", "Basic", "Optimized", "Weighted" };
    }

    inline juce::StringArray getDisplayModeNames()
    {
        return { "SlowDown", "Fast", "Slow", "Infinite", "Off" };
    }

    inline juce::StringArray getMeterScaleNames()
    {
        return { "16 dB", "32 dB", "48 dB", "K-12", "K-14", "K-20" };
    }

    inline juce::StringArray getLoudnessTargetNames()
    {
        return { "Off", "-23 LUFS", "-16 LUFS", "-14 LUFS" };
    }

    inline juce::StringArray getLoudnessTimeNames()
    {
        return { "Momentary", "Short Term", "Integrated" };
    }
}

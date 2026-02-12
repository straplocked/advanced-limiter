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

#include "ParameterLayout.h"
#include "ParameterIDs.h"
#include "../utils/Constants.h"

namespace ParameterLayout
{

juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // ---- Input / Output ----

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::kInputGain, 1 },
        "Input Gain",
        juce::NormalisableRange<float> (Constants::kMinInputGain, Constants::kMaxInputGain, 0.01f),
        Constants::kDefaultInputGain));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::kOutputLevel, 1 },
        "Output Level",
        juce::NormalisableRange<float> (Constants::kMinOutputLevel, Constants::kMaxOutputLevel, 0.01f),
        Constants::kDefaultOutputLevel));

    // ---- Limiter Style ----

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::kLimiterStyle, 1 },
        "Style",
        ParamIDs::getStyleNames(),
        4));  // default Modern

    // ---- Timing ----

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::kLookahead, 1 },
        "Lookahead",
        juce::NormalisableRange<float> (Constants::kMinLookahead, Constants::kMaxLookahead, 0.001f, 0.5f),
        Constants::kDefaultLookahead));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::kAttack, 1 },
        "Attack",
        juce::NormalisableRange<float> (Constants::kMinAttack, Constants::kMaxAttack, 0.001f, 0.3f),
        Constants::kDefaultAttack));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::kRelease, 1 },
        "Release",
        juce::NormalisableRange<float> (Constants::kMinRelease, Constants::kMaxRelease, 0.1f, 0.3f),
        Constants::kDefaultRelease));

    // ---- Link ----

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::kTransientLink, 1 },
        "Transient Link",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        Constants::kDefaultTransientLink));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::kReleaseLink, 1 },
        "Release Link",
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f),
        Constants::kDefaultReleaseLink));

    // ---- Detection ----

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::kTruePeak, 1 },
        "True Peak",
        true));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::kDCFilter, 1 },
        "DC Filter",
        false));

    // ---- Dither ----

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::kDitherOn, 1 },
        "Dither",
        false));

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::kDitherBits, 1 },
        "Dither Bits",
        ParamIDs::getDitherBitNames(),
        0));  // default 16 bit

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::kNoiseShape, 1 },
        "Noise Shaping",
        ParamIDs::getNoiseShapeNames(),
        0));  // default Off

    // ---- Oversampling ----

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::kOversample, 1 },
        "Oversampling",
        ParamIDs::getOversamplingNames(),
        0));  // default 1x

    // ---- Sidechain / Monitoring ----

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::kSCExternal, 1 },
        "SC External",
        false));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::kUnityGain, 1 },
        "Unity Gain",
        false));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::kAudition, 1 },
        "Audition",
        false));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::kDelta, 1 },
        "Delta",
        false));

    // ---- Display ----

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::kDisplayMode, 1 },
        "Display Mode",
        ParamIDs::getDisplayModeNames(),
        1));  // default Fast

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::kMeterScale, 1 },
        "Meter Scale",
        ParamIDs::getMeterScaleNames(),
        Constants::kDefaultMeterScaleIndex));  // default 32 dB

    // ---- Loudness ----

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::kLoudnessTarget, 1 },
        "Loudness Target",
        ParamIDs::getLoudnessTargetNames(),
        0));  // default Off

    layout.add (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { ParamIDs::kLoudnessTime, 1 },
        "Loudness Time",
        ParamIDs::getLoudnessTimeNames(),
        0));  // default Momentary

    // ---- Global ----

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::kBypass, 1 },
        "Bypass",
        false));

    return layout;
}

} // namespace ParameterLayout

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

namespace Constants
{
    constexpr int kDefaultEditorWidth = 1056;
    constexpr int kDefaultEditorHeight = 726;
    constexpr int kUITimerHz = 30;

    // Layout constants (1.1x scaled)
    constexpr int kHeaderHeight = 40;
    constexpr int kBottomStripHeight = 154;
    constexpr int kInputGainStripWidth = 50;
    constexpr int kMeterWidth = 110;
    constexpr int kLoudnessPanelHeight = 33;

    // Limiter parameter ranges
    constexpr float kMinInputGain = -30.0f;
    constexpr float kMaxInputGain = 30.0f;
    constexpr float kDefaultInputGain = 0.0f;
    constexpr float kMinOutputLevel = -30.0f;
    constexpr float kMaxOutputLevel = 0.0f;
    constexpr float kDefaultOutputLevel = -0.3f;
    constexpr float kMinLookahead = 0.05f;
    constexpr float kMaxLookahead = 5.0f;
    constexpr float kDefaultLookahead = 2.0f;
    constexpr float kMinAttack = 0.01f;
    constexpr float kMaxAttack = 10.0f;
    constexpr float kDefaultAttack = 0.1f;
    constexpr float kMinRelease = 10.0f;
    constexpr float kMaxRelease = 1000.0f;
    constexpr float kDefaultRelease = 100.0f;
    constexpr float kDefaultTransientLink = 100.0f;
    constexpr float kDefaultReleaseLink = 100.0f;

    // Meter
    constexpr float kMeterMinDB = -60.0f;
    constexpr float kMeterMaxDB = 6.0f;
    constexpr float kGRMeterMinDB = -40.0f;
    constexpr float kGRMeterMaxDB = 0.0f;
    constexpr float kPeakHoldTimeMs = 1500.0f;
    constexpr float kPeakDecayDBPerSec = 20.0f;

    // Meter scale ranges
    constexpr float kMeterScaleRanges[] = { 16.0f, 32.0f, 48.0f, 12.0f, 14.0f, 20.0f };
    constexpr int kDefaultMeterScaleIndex = 1; // 32 dB

    // Max buffer sizes
    constexpr float kMaxLookaheadMs = 10.0f;
    constexpr int kNumStyles = 8;

    // Scrolling display
    constexpr int kScrollingHistorySize = 2048;

    // DC filter
    constexpr float kDCFilterFreq = 5.0f;
}

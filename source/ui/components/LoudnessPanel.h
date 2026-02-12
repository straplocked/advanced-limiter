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

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../metering/LevelMeterData.h"
#include "../LookAndFeel/ColourPalette.h"
#include "../LookAndFeel/Fonts.h"

class LoudnessPanel : public juce::Component,
                      public juce::Timer
{
public:
    explicit LoudnessPanel (LevelMeterData& meterData);
    ~LoudnessPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void setLoudnessTarget (int target);   // 0=Off, 1=-23, 2=-16, 3=-14
    void setLoudnessValues (float m, float s, float i, float range);

    std::function<void()> onResetClicked;

private:
    LevelMeterData& meterData;
    juce::TextButton resetButton { "Rst" };

    float momentary = -100.0f;
    float shortTerm = -100.0f;
    float integrated = -100.0f;
    float lra = 0.0f;
    int loudnessTarget = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoudnessPanel)
};

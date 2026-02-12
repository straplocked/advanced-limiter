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
#include "../../metering/ScrollingLevelHistory.h"
#include "../LookAndFeel/ColourPalette.h"
#include "../LookAndFeel/Fonts.h"

class LimiterDisplay : public juce::Component,
                       public juce::Timer
{
public:
    explicit LimiterDisplay (LevelMeterData& meterData);
    ~LimiterDisplay() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void setDisplayMode (int mode);   // 0=SlowDown, 1=Fast, 2=Slow, 3=Infinite, 4=Off
    void setMeterScale (int scale);   // 0-5

private:
    LevelMeterData& meterData;
    ScrollingLevelHistory history;

    int displayMode = 1;   // Fast
    int meterScale = 1;    // 32 dB

    float getScaleRange() const;
    void drawGrid (juce::Graphics& g, juce::Rectangle<float> area);
    void drawScrollingData (juce::Graphics& g, juce::Rectangle<float> area);
    void drawGRPeakLabels (juce::Graphics& g, juce::Rectangle<float> area);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LimiterDisplay)
};

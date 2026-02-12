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
#include "../../metering/PeakHoldCalculator.h"
#include "../LookAndFeel/ColourPalette.h"
#include "../LookAndFeel/Fonts.h"

class LevelMeterComponent : public juce::Component,
                            public juce::Timer
{
public:
    explicit LevelMeterComponent (LevelMeterData& meterData);
    ~LevelMeterComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void setMeterScale (int scale);

private:
    LevelMeterData& meterData;
    int meterScale = 1;

    // Smoothed values
    float smoothInputL = -60.0f, smoothInputR = -60.0f;
    float smoothOutputL = -60.0f, smoothOutputR = -60.0f;
    float smoothGR = 0.0f;
    float truePeakL = -60.0f, truePeakR = -60.0f;

    // Peak hold
    PeakHoldCalculator peakHoldInputL, peakHoldInputR;
    PeakHoldCalculator peakHoldOutputL, peakHoldOutputR;

    void drawMeterBar (juce::Graphics& g, juce::Rectangle<float> area,
                       float valueDB, float peakDB, bool isGR);
    void drawTruePeakMarker (juce::Graphics& g, juce::Rectangle<float> area, float tpDB);
    float getScaleRange() const;
    float dbToY (float db, float height) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeterComponent)
};

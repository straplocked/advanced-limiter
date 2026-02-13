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
#include <juce_audio_processors/juce_audio_processors.h>
#include "../LookAndFeel/ColourPalette.h"
#include "../LookAndFeel/Fonts.h"

class InputGainStrip : public juce::Component
{
public:
    explicit InputGainStrip (juce::AudioProcessorValueTreeState& apvts);
    ~InputGainStrip() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider gainSlider;
    juce::Label gainLabel;
    juce::Label gainValue;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAtt;

    void updateValueLabel();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputGainStrip)
};

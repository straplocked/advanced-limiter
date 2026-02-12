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

class ControlTray : public juce::Component
{
public:
    explicit ControlTray (juce::AudioProcessorValueTreeState& apvts);
    ~ControlTray() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Slider lookaheadKnob, attackKnob, releaseKnob, transientLinkKnob, releaseLinkKnob;
    juce::Label lookaheadLabel, attackLabel, releaseLabel, transientLinkLabel, releaseLinkLabel;
    juce::Label lookaheadValue, attackValue, releaseValue, transientLinkValue, releaseLinkValue;

    juce::ComboBox styleCombo, displayModeCombo;
    juce::Label styleLabel, displayLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lookaheadAtt, attackAtt, releaseAtt, transLinkAtt, relLinkAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> styleAtt, displayAtt;

    void setupKnob (juce::Slider& knob, juce::Label& label, juce::Label& value, const juce::String& text);
    void updateValueLabels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlTray)
};

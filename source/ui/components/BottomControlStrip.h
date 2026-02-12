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

class BottomControlStrip : public juce::Component
{
public:
    explicit BottomControlStrip (juce::AudioProcessorValueTreeState& apvts);
    ~BottomControlStrip() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    std::function<void()> onControlsToggle;
    void setControlTrayOpen (bool open);

private:
    juce::Slider inputGainKnob, outputLevelKnob;
    juce::Label inputGainLabel, outputLevelLabel;
    juce::Label inputGainValue, outputLevelValue;

    juce::TextButton truePeakButton { "TP" };
    juce::TextButton dcFilterButton { "DC" };
    juce::TextButton unityGainButton { "UG" };
    juce::TextButton auditionButton { "Aud" };
    juce::TextButton scExternalButton { "SC" };
    juce::TextButton controlsToggle { "CONTROLS" };

    juce::ComboBox ditherBitsCombo, noiseShapeCombo, oversampleCombo, meterScaleCombo;
    juce::TextButton ditherOnButton { "Dither" };

    juce::Label ditherBitsLabel, noiseShapeLabel, oversampleLabel, meterScaleLabel;

    // APVTS attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAtt, outputLevelAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tpAtt, dcAtt, ugAtt, audAtt, scAtt, ditherAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ditherBitsAtt, noiseShapeAtt, oversampleAtt, meterScaleAtt;

    bool controlTrayOpen = false;

    void setupKnob (juce::Slider& knob, juce::Label& label, juce::Label& value, const juce::String& text);
    void setupToggleButton (juce::TextButton& button, const juce::String& id);
    void setupCombo (juce::ComboBox& combo, juce::Label& label, const juce::String& text,
                     const juce::StringArray& items, const juce::String& id);
    void updateValueLabels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomControlStrip)
};

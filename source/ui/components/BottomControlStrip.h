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

private:
    // Row 1: Style combo, 5 knobs, Display combo
    juce::Slider lookaheadKnob, attackKnob, releaseKnob, transientLinkKnob, releaseLinkKnob;
    juce::Label lookaheadLabel, attackLabel, releaseLabel, transientLinkLabel, releaseLinkLabel;
    juce::Label lookaheadValue, attackValue, releaseValue, transientLinkValue, releaseLinkValue;

    juce::ComboBox styleCombo, displayModeCombo;
    juce::Label styleLabel, displayLabel;

    // Row 2: Toggle buttons
    juce::TextButton truePeakButton { "TP" };
    juce::TextButton dcFilterButton { "DC" };
    juce::TextButton unityGainButton { "UG" };
    juce::TextButton auditionButton { "Aud" };
    juce::TextButton scExternalButton { "SC" };
    juce::TextButton midSideButton { "M/S" };

    // Row 3: Dither, combos, output level
    juce::Slider outputLevelKnob;
    juce::Label outputLevelLabel;
    juce::Label outputLevelValue;

    juce::ComboBox ditherBitsCombo, noiseShapeCombo, oversampleCombo, meterScaleCombo;
    juce::TextButton ditherOnButton { "Dither" };
    juce::Label ditherBitsLabel, noiseShapeLabel, oversampleLabel, meterScaleLabel;

    // APVTS attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        outputLevelAtt, lookaheadAtt, attackAtt, releaseAtt, transLinkAtt, relLinkAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        tpAtt, dcAtt, ugAtt, audAtt, scAtt, msAtt, ditherAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        styleAtt, displayAtt, ditherBitsAtt, noiseShapeAtt, oversampleAtt, meterScaleAtt;

    void setupKnob (juce::Slider& knob, juce::Label& label, juce::Label& value, const juce::String& text);
    void setupToggleButton (juce::TextButton& button, const juce::String& id);
    void setupCombo (juce::ComboBox& combo, juce::Label& label, const juce::String& text,
                     const juce::StringArray& items, const juce::String& id);
    void updateValueLabels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BottomControlStrip)
};

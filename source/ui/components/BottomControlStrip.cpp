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

#include "BottomControlStrip.h"
#include "../../parameters/ParameterIDs.h"

BottomControlStrip::BottomControlStrip (juce::AudioProcessorValueTreeState& apvts)
{
    // Input Gain knob
    setupKnob (inputGainKnob, inputGainLabel, inputGainValue, "Input");
    inputGainKnob.setComponentID ("bottom.inputGain");
    inputGainKnob.onValueChange = [this] { updateValueLabels(); };

    // Output Level knob
    setupKnob (outputLevelKnob, outputLevelLabel, outputLevelValue, "Output");
    outputLevelKnob.setComponentID ("bottom.outputLevel");
    outputLevelKnob.onValueChange = [this] { updateValueLabels(); };

    // Toggle buttons
    setupToggleButton (truePeakButton, "bottom.truePeak");
    setupToggleButton (dcFilterButton, "bottom.dcFilter");
    setupToggleButton (unityGainButton, "bottom.unityGain");
    setupToggleButton (auditionButton, "bottom.audition");
    setupToggleButton (scExternalButton, "bottom.scExternal");
    setupToggleButton (ditherOnButton, "bottom.dither");

    // Controls toggle (non-APVTS)
    controlsToggle.setComponentID ("bottom.controlsToggle");
    controlsToggle.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    controlsToggle.setColour (juce::TextButton::textColourOffId, ColourPalette::textDim());
    controlsToggle.setColour (juce::TextButton::buttonOnColourId, ColourPalette::gainReduction().withAlpha (0.2f));
    controlsToggle.setColour (juce::TextButton::textColourOnId, ColourPalette::textBright());
    controlsToggle.setClickingTogglesState (true);
    controlsToggle.onClick = [this]
    {
        if (onControlsToggle)
            onControlsToggle();
    };
    addAndMakeVisible (controlsToggle);

    // Combo boxes
    setupCombo (ditherBitsCombo, ditherBitsLabel, "Bits", ParamIDs::getDitherBitNames(), "bottom.ditherBits");
    setupCombo (noiseShapeCombo, noiseShapeLabel, "Shape", ParamIDs::getNoiseShapeNames(), "bottom.noiseShape");
    setupCombo (oversampleCombo, oversampleLabel, "OS", ParamIDs::getOversamplingNames(), "bottom.oversample");
    setupCombo (meterScaleCombo, meterScaleLabel, "Meter", ParamIDs::getMeterScaleNames(), "bottom.meterScale");

    // APVTS attachments
    inputGainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kInputGain, inputGainKnob);
    outputLevelAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kOutputLevel, outputLevelKnob);

    tpAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, ParamIDs::kTruePeak, truePeakButton);
    dcAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, ParamIDs::kDCFilter, dcFilterButton);
    ugAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, ParamIDs::kUnityGain, unityGainButton);
    audAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, ParamIDs::kAudition, auditionButton);
    scAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, ParamIDs::kSCExternal, scExternalButton);
    ditherAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, ParamIDs::kDitherOn, ditherOnButton);

    ditherBitsAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, ParamIDs::kDitherBits, ditherBitsCombo);
    noiseShapeAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, ParamIDs::kNoiseShape, noiseShapeCombo);
    oversampleAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, ParamIDs::kOversample, oversampleCombo);
    meterScaleAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, ParamIDs::kMeterScale, meterScaleCombo);

    updateValueLabels();
}

BottomControlStrip::~BottomControlStrip()
{
    inputGainAtt.reset(); outputLevelAtt.reset();
    tpAtt.reset(); dcAtt.reset(); ugAtt.reset(); audAtt.reset(); scAtt.reset(); ditherAtt.reset();
    ditherBitsAtt.reset(); noiseShapeAtt.reset(); oversampleAtt.reset(); meterScaleAtt.reset();
}

void BottomControlStrip::setupKnob (juce::Slider& knob, juce::Label& label, juce::Label& value,
                                     const juce::String& text)
{
    knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible (knob);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setFont (Fonts::getBody (10.0f));
    label.setColour (juce::Label::textColourId, ColourPalette::textDim());
    addAndMakeVisible (label);

    value.setJustificationType (juce::Justification::centred);
    value.setFont (Fonts::getBody (11.0f));
    value.setColour (juce::Label::textColourId, ColourPalette::axisText());
    addAndMakeVisible (value);
}

void BottomControlStrip::setupToggleButton (juce::TextButton& button, const juce::String& id)
{
    button.setComponentID (id);
    button.setClickingTogglesState (true);
    button.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    button.setColour (juce::TextButton::textColourOffId, ColourPalette::textDim());
    button.setColour (juce::TextButton::buttonOnColourId, ColourPalette::gainReduction().withAlpha (0.25f));
    button.setColour (juce::TextButton::textColourOnId, ColourPalette::textBright());
    addAndMakeVisible (button);
}

void BottomControlStrip::setupCombo (juce::ComboBox& combo, juce::Label& label,
                                      const juce::String& text, const juce::StringArray& items,
                                      const juce::String& id)
{
    combo.addItemList (items, 1);
    combo.setJustificationType (juce::Justification::centred);
    combo.setComponentID (id);
    addAndMakeVisible (combo);

    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centredRight);
    label.setFont (Fonts::getBody (11.0f));
    label.setColour (juce::Label::textColourId, ColourPalette::axisText());
    addAndMakeVisible (label);
}

void BottomControlStrip::updateValueLabels()
{
    inputGainValue.setText (juce::String (inputGainKnob.getValue(), 1) + " dB", juce::dontSendNotification);
    outputLevelValue.setText (juce::String (outputLevelKnob.getValue(), 1) + " dB", juce::dontSendNotification);
}

void BottomControlStrip::setControlTrayOpen (bool open)
{
    controlTrayOpen = open;
    controlsToggle.setToggleState (open, juce::dontSendNotification);
}

void BottomControlStrip::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Panel gradient
    juce::ColourGradient grad (ColourPalette::panelTop(), 0, bounds.getY(),
                               ColourPalette::panelBottom(), 0, bounds.getBottom(), false);
    g.setGradientFill (grad);
    g.fillRect (bounds);

    // Top bevel
    g.setColour (ColourPalette::bevelLight());
    g.drawLine (0.0f, bounds.getY() + 0.5f, bounds.getRight(), bounds.getY() + 0.5f, 1.0f);
    g.setColour (ColourPalette::bevelDark());
    g.drawLine (0.0f, bounds.getY() + 1.5f, bounds.getRight(), bounds.getY() + 1.5f, 1.0f);

    // Separator between rows
    float midY = bounds.getCentreY();
    g.setColour (ColourPalette::gridLine().withAlpha (0.5f));
    g.drawLine (8.0f, midY, bounds.getRight() - 8.0f, midY, 0.5f);
}

void BottomControlStrip::resized()
{
    auto bounds = getLocalBounds();

    // Two rows: top row = knobs + toggle buttons, bottom row = combos + CONTROLS
    auto topRow = bounds.removeFromTop (bounds.getHeight() / 2);
    auto bottomRow = bounds;

    // === Top row: Input Gain knob, toggle buttons, Output Level knob ===
    {
        auto area = topRow.reduced (8, 2);
        int knobW = 60;
        int btnH = 20;
        int btnW = 32;
        int gap = 6;

        // Input Gain knob on left
        auto inputCol = area.removeFromLeft (knobW);
        inputGainLabel.setBounds (inputCol.removeFromTop (12));
        inputGainValue.setBounds (inputCol.removeFromBottom (12));
        inputGainKnob.setBounds (inputCol);

        area.removeFromLeft (gap);

        // Output Level knob on right
        auto outputCol = area.removeFromRight (knobW);
        outputLevelLabel.setBounds (outputCol.removeFromTop (12));
        outputLevelValue.setBounds (outputCol.removeFromBottom (12));
        outputLevelKnob.setBounds (outputCol);

        area.removeFromRight (gap);

        // Toggle buttons in the center
        auto btnArea = area.withSizeKeepingCentre (
            (btnW + gap) * 5 - gap, btnH);

        truePeakButton.setBounds (btnArea.removeFromLeft (btnW));
        btnArea.removeFromLeft (gap);
        dcFilterButton.setBounds (btnArea.removeFromLeft (btnW));
        btnArea.removeFromLeft (gap);
        unityGainButton.setBounds (btnArea.removeFromLeft (btnW));
        btnArea.removeFromLeft (gap);
        auditionButton.setBounds (btnArea.removeFromLeft (btnW));
        btnArea.removeFromLeft (gap);
        scExternalButton.setBounds (btnArea.removeFromLeft (btnW));
    }

    // === Bottom row: Dither, combos, CONTROLS button ===
    {
        auto area = bottomRow.reduced (8, 2);
        int comboH = 20;
        int comboW = 58;
        int btnW = 40;
        int lblW = 34;
        int gap = 8;

        // CONTROLS button on right
        auto controlsArea = area.removeFromRight (70);
        controlsToggle.setBounds (controlsArea.withHeight (comboH).withCentre (controlsArea.getCentre()));
        area.removeFromRight (gap);

        // Center: Dither, Bits, Shape, OS, Meter
        int totalComboW = btnW + gap + (lblW + comboW + gap) * 4;
        auto comboArea = area.withSizeKeepingCentre (totalComboW, area.getHeight());

        // Dither toggle
        auto ditherArea = comboArea.removeFromLeft (btnW);
        ditherOnButton.setBounds (ditherArea.withHeight (comboH).withCentre (ditherArea.getCentre()));
        comboArea.removeFromLeft (gap);

        // Dither bits
        auto bitsLbl = comboArea.removeFromLeft (lblW);
        ditherBitsLabel.setBounds (bitsLbl.withHeight (comboH).withCentre (bitsLbl.getCentre()));
        auto bitsArea = comboArea.removeFromLeft (comboW);
        ditherBitsCombo.setBounds (bitsArea.withHeight (comboH).withCentre (bitsArea.getCentre()));
        comboArea.removeFromLeft (gap);

        // Noise Shape
        auto shapeLbl = comboArea.removeFromLeft (lblW);
        noiseShapeLabel.setBounds (shapeLbl.withHeight (comboH).withCentre (shapeLbl.getCentre()));
        auto shapeArea = comboArea.removeFromLeft (comboW);
        noiseShapeCombo.setBounds (shapeArea.withHeight (comboH).withCentre (shapeArea.getCentre()));
        comboArea.removeFromLeft (gap);

        // Oversample
        auto osLbl = comboArea.removeFromLeft (lblW);
        oversampleLabel.setBounds (osLbl.withHeight (comboH).withCentre (osLbl.getCentre()));
        auto osArea = comboArea.removeFromLeft (comboW);
        oversampleCombo.setBounds (osArea.withHeight (comboH).withCentre (osArea.getCentre()));
        comboArea.removeFromLeft (gap);

        // Meter Scale
        auto mLbl = comboArea.removeFromLeft (lblW);
        meterScaleLabel.setBounds (mLbl.withHeight (comboH).withCentre (mLbl.getCentre()));
        auto mArea = comboArea.removeFromLeft (comboW);
        meterScaleCombo.setBounds (mArea.withHeight (comboH).withCentre (mArea.getCentre()));
    }
}

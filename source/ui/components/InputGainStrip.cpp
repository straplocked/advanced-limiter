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

#include "InputGainStrip.h"
#include "../../parameters/ParameterIDs.h"

InputGainStrip::InputGainStrip (juce::AudioProcessorValueTreeState& apvts)
{
    gainSlider.setSliderStyle (juce::Slider::LinearVertical);
    gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    gainSlider.setComponentID ("bottom.inputGain");
    gainSlider.onValueChange = [this] { updateValueLabel(); };
    addAndMakeVisible (gainSlider);

    gainLabel.setText ("GAIN", juce::dontSendNotification);
    gainLabel.setJustificationType (juce::Justification::centred);
    gainLabel.setFont (Fonts::getBody (9.0f));
    gainLabel.setColour (juce::Label::textColourId, ColourPalette::textDim());
    addAndMakeVisible (gainLabel);

    gainValue.setJustificationType (juce::Justification::centred);
    gainValue.setFont (Fonts::getBody (9.0f));
    gainValue.setColour (juce::Label::textColourId, ColourPalette::axisText());
    addAndMakeVisible (gainValue);

    gainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, ParamIDs::kInputGain, gainSlider);

    updateValueLabel();
}

InputGainStrip::~InputGainStrip()
{
    gainAtt.reset();
}

void InputGainStrip::updateValueLabel()
{
    float val = (float) gainSlider.getValue();
    juce::String text;
    if (val > 0.0f)
        text = "+" + juce::String (val, 1);
    else
        text = juce::String (val, 1);
    gainValue.setText (text, juce::dontSendNotification);
}

void InputGainStrip::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Dark panel gradient background
    juce::ColourGradient grad (ColourPalette::panelTop(), 0, bounds.getY(),
                               ColourPalette::panelBottom(), 0, bounds.getBottom(), false);
    g.setGradientFill (grad);
    g.fillRect (bounds);

    // Right bevel edge
    g.setColour (ColourPalette::bevelLight());
    g.drawLine (bounds.getRight() - 1.5f, bounds.getY(), bounds.getRight() - 1.5f, bounds.getBottom(), 1.0f);
    g.setColour (ColourPalette::bevelDark());
    g.drawLine (bounds.getRight() - 0.5f, bounds.getY(), bounds.getRight() - 0.5f, bounds.getBottom(), 1.0f);
}

void InputGainStrip::resized()
{
    auto bounds = getLocalBounds().reduced (4, 6);

    // Value readout at top
    gainValue.setBounds (bounds.removeFromTop (14));

    // Label at bottom
    gainLabel.setBounds (bounds.removeFromBottom (14));

    // Slider fills the middle
    bounds.reduce (6, 4);
    gainSlider.setBounds (bounds);
}

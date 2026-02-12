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

#include "ControlTray.h"
#include "../../parameters/ParameterIDs.h"

ControlTray::ControlTray (juce::AudioProcessorValueTreeState& apvts)
{
    setupKnob (lookaheadKnob, lookaheadLabel, lookaheadValue, "Lookahead");
    setupKnob (attackKnob, attackLabel, attackValue, "Attack");
    setupKnob (releaseKnob, releaseLabel, releaseValue, "Release");
    setupKnob (transientLinkKnob, transientLinkLabel, transientLinkValue, "Trans Link");
    setupKnob (releaseLinkKnob, releaseLinkLabel, releaseLinkValue, "Rel Link");

    lookaheadKnob.setComponentID ("controls.lookahead");
    attackKnob.setComponentID ("controls.attack");
    releaseKnob.setComponentID ("controls.release");
    transientLinkKnob.setComponentID ("controls.transientLink");
    releaseLinkKnob.setComponentID ("controls.releaseLink");

    lookaheadKnob.onValueChange = [this] { updateValueLabels(); };
    attackKnob.onValueChange = [this] { updateValueLabels(); };
    releaseKnob.onValueChange = [this] { updateValueLabels(); };
    transientLinkKnob.onValueChange = [this] { updateValueLabels(); };
    releaseLinkKnob.onValueChange = [this] { updateValueLabels(); };

    // Style combo
    styleCombo.addItemList (ParamIDs::getStyleNames(), 1);
    styleCombo.setJustificationType (juce::Justification::centred);
    styleCombo.setComponentID ("controls.style");
    addAndMakeVisible (styleCombo);
    styleLabel.setText ("Style", juce::dontSendNotification);
    styleLabel.setJustificationType (juce::Justification::centred);
    styleLabel.setFont (Fonts::getBody (10.0f));
    styleLabel.setColour (juce::Label::textColourId, ColourPalette::textDim());
    addAndMakeVisible (styleLabel);

    // Display mode combo
    displayModeCombo.addItemList (ParamIDs::getDisplayModeNames(), 1);
    displayModeCombo.setJustificationType (juce::Justification::centred);
    displayModeCombo.setComponentID ("controls.displayMode");
    addAndMakeVisible (displayModeCombo);
    displayLabel.setText ("Display", juce::dontSendNotification);
    displayLabel.setJustificationType (juce::Justification::centred);
    displayLabel.setFont (Fonts::getBody (10.0f));
    displayLabel.setColour (juce::Label::textColourId, ColourPalette::textDim());
    addAndMakeVisible (displayLabel);

    // APVTS attachments
    lookaheadAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kLookahead, lookaheadKnob);
    attackAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kAttack, attackKnob);
    releaseAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kRelease, releaseKnob);
    transLinkAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kTransientLink, transientLinkKnob);
    relLinkAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kReleaseLink, releaseLinkKnob);
    styleAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, ParamIDs::kLimiterStyle, styleCombo);
    displayAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, ParamIDs::kDisplayMode, displayModeCombo);

    updateValueLabels();
}

ControlTray::~ControlTray()
{
    lookaheadAtt.reset(); attackAtt.reset(); releaseAtt.reset();
    transLinkAtt.reset(); relLinkAtt.reset();
    styleAtt.reset(); displayAtt.reset();
}

void ControlTray::setupKnob (juce::Slider& knob, juce::Label& label, juce::Label& value,
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

void ControlTray::updateValueLabels()
{
    lookaheadValue.setText (juce::String (lookaheadKnob.getValue(), 2) + " ms", juce::dontSendNotification);
    attackValue.setText (juce::String (attackKnob.getValue(), 2) + " ms", juce::dontSendNotification);
    releaseValue.setText (juce::String (releaseKnob.getValue(), 0) + " ms", juce::dontSendNotification);
    transientLinkValue.setText (juce::String ((int) transientLinkKnob.getValue()) + "%", juce::dontSendNotification);
    releaseLinkValue.setText (juce::String ((int) releaseLinkKnob.getValue()) + "%", juce::dontSendNotification);
}

void ControlTray::paint (juce::Graphics& g)
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

    // Bottom bevel
    g.setColour (ColourPalette::bevelLight());
    g.drawLine (0.0f, bounds.getBottom() - 1.5f, bounds.getRight(), bounds.getBottom() - 1.5f, 1.0f);
    g.setColour (ColourPalette::bevelDark());
    g.drawLine (0.0f, bounds.getBottom() - 0.5f, bounds.getRight(), bounds.getBottom() - 0.5f, 1.0f);
}

void ControlTray::resized()
{
    auto bounds = getLocalBounds().reduced (0, 4);

    // Top row: 5 knobs
    int knobW = 72;
    int knobRowH = 90;
    int totalKnobW = knobW * 5;
    auto knobRow = bounds.removeFromTop (knobRowH)
                          .withSizeKeepingCentre (totalKnobW, knobRowH);

    auto layoutKnob = [] (juce::Slider& slider, juce::Label& title, juce::Label& value,
                           juce::Rectangle<int>& r, int w)
    {
        auto col = r.removeFromLeft (w);
        title.setBounds (col.removeFromTop (14));
        value.setBounds (col.removeFromBottom (14));
        slider.setBounds (col);
    };

    layoutKnob (lookaheadKnob, lookaheadLabel, lookaheadValue, knobRow, knobW);
    layoutKnob (attackKnob, attackLabel, attackValue, knobRow, knobW);
    layoutKnob (releaseKnob, releaseLabel, releaseValue, knobRow, knobW);
    layoutKnob (transientLinkKnob, transientLinkLabel, transientLinkValue, knobRow, knobW);
    layoutKnob (releaseLinkKnob, releaseLinkLabel, releaseLinkValue, knobRow, knobW);

    // Bottom row: 2 combos
    bounds.removeFromTop (4);
    auto comboRow = bounds.withSizeKeepingCentre (280, 22);
    int comboW = 100;
    int gap = 20;

    auto styleArea = comboRow.removeFromLeft (comboW);
    auto styleLblArea = comboRow.removeFromLeft (40);
    comboRow.removeFromLeft (gap);
    auto displayArea = comboRow.removeFromLeft (comboW);
    auto displayLblArea = comboRow.removeFromLeft (40);

    styleCombo.setBounds (styleArea);
    styleLabel.setBounds (styleLblArea);
    displayModeCombo.setBounds (displayArea);
    displayLabel.setBounds (displayLblArea);
}

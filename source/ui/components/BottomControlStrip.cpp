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
    // === Row 1: Knobs (from ControlTray) ===
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
    styleLabel.setJustificationType (juce::Justification::centredRight);
    styleLabel.setFont (Fonts::getBody (10.0f));
    styleLabel.setColour (juce::Label::textColourId, ColourPalette::textDim());
    addAndMakeVisible (styleLabel);

    // Display mode combo
    displayModeCombo.addItemList (ParamIDs::getDisplayModeNames(), 1);
    displayModeCombo.setJustificationType (juce::Justification::centred);
    displayModeCombo.setComponentID ("controls.displayMode");
    addAndMakeVisible (displayModeCombo);
    displayLabel.setText ("Display", juce::dontSendNotification);
    displayLabel.setJustificationType (juce::Justification::centredRight);
    displayLabel.setFont (Fonts::getBody (10.0f));
    displayLabel.setColour (juce::Label::textColourId, ColourPalette::textDim());
    addAndMakeVisible (displayLabel);

    // === Row 2: Toggle buttons ===
    setupToggleButton (truePeakButton, "bottom.truePeak");
    setupToggleButton (dcFilterButton, "bottom.dcFilter");
    setupToggleButton (unityGainButton, "bottom.unityGain");
    setupToggleButton (auditionButton, "bottom.audition");
    setupToggleButton (scExternalButton, "bottom.scExternal");

    // === Row 3: Output Level knob, Dither controls, combos ===
    setupKnob (outputLevelKnob, outputLevelLabel, outputLevelValue, "Output");
    outputLevelKnob.setComponentID ("bottom.outputLevel");
    outputLevelKnob.onValueChange = [this] { updateValueLabels(); };

    setupToggleButton (ditherOnButton, "bottom.dither");

    setupCombo (ditherBitsCombo, ditherBitsLabel, "Bits", ParamIDs::getDitherBitNames(), "bottom.ditherBits");
    setupCombo (noiseShapeCombo, noiseShapeLabel, "Shape", ParamIDs::getNoiseShapeNames(), "bottom.noiseShape");
    setupCombo (oversampleCombo, oversampleLabel, "OS", ParamIDs::getOversamplingNames(), "bottom.oversample");
    setupCombo (meterScaleCombo, meterScaleLabel, "Meter", ParamIDs::getMeterScaleNames(), "bottom.meterScale");

    // === APVTS attachments ===
    outputLevelAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kOutputLevel, outputLevelKnob);

    lookaheadAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kLookahead, lookaheadKnob);
    attackAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kAttack, attackKnob);
    releaseAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kRelease, releaseKnob);
    transLinkAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kTransientLink, transientLinkKnob);
    relLinkAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, ParamIDs::kReleaseLink, releaseLinkKnob);

    styleAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, ParamIDs::kLimiterStyle, styleCombo);
    displayAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (apvts, ParamIDs::kDisplayMode, displayModeCombo);

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
    outputLevelAtt.reset();
    lookaheadAtt.reset(); attackAtt.reset(); releaseAtt.reset();
    transLinkAtt.reset(); relLinkAtt.reset();
    styleAtt.reset(); displayAtt.reset();
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
    label.setFont (Fonts::getBody (11.0f));
    label.setColour (juce::Label::textColourId, ColourPalette::textDim());
    addAndMakeVisible (label);

    value.setJustificationType (juce::Justification::centred);
    value.setFont (Fonts::getBody (12.0f));
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
    outputLevelValue.setText (juce::String (outputLevelKnob.getValue(), 1) + " dB", juce::dontSendNotification);
    lookaheadValue.setText (juce::String (lookaheadKnob.getValue(), 2) + " ms", juce::dontSendNotification);
    attackValue.setText (juce::String (attackKnob.getValue(), 2) + " ms", juce::dontSendNotification);
    releaseValue.setText (juce::String (releaseKnob.getValue(), 0) + " ms", juce::dontSendNotification);
    transientLinkValue.setText (juce::String ((int) transientLinkKnob.getValue()) + "%", juce::dontSendNotification);
    releaseLinkValue.setText (juce::String ((int) releaseLinkKnob.getValue()) + "%", juce::dontSendNotification);
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

    // Row separator between main knobs and bottom bar
    float row1H = bounds.getHeight() - 38.0f;
    g.setColour (ColourPalette::gridLine().withAlpha (0.5f));
    g.drawLine (8.0f, bounds.getY() + row1H, bounds.getRight() - 8.0f,
                bounds.getY() + row1H, 0.5f);
}

void BottomControlStrip::resized()
{
    auto bounds = getLocalBounds();

    // Two rows: Row 1 = main knobs (large), Row 2 = toggle buttons left + combos right (38px)
    int bottomBarH = 38;
    auto bottomBar = bounds.removeFromBottom (bottomBarH);
    auto row1 = bounds;

    // === Row 1: Style | Lookahead | Attack | Release | Trans Link | Rel Link | Output | Display ===
    {
        auto area = row1.reduced (8, 2);
        int knobW = 80;
        int comboW = 90;
        int lblW = 40;
        int gap = 4;

        // Style combo on left
        auto styleArea = area.removeFromLeft (lblW);
        styleLabel.setBounds (styleArea.withHeight (22).withCentre (styleArea.getCentre()));
        area.removeFromLeft (gap);
        auto styleComboArea = area.removeFromLeft (comboW);
        styleCombo.setBounds (styleComboArea.withHeight (22).withCentre (styleComboArea.getCentre()));
        area.removeFromLeft (gap * 2);

        // Display combo on right
        auto displayComboArea = area.removeFromRight (comboW);
        displayModeCombo.setBounds (displayComboArea.withHeight (22).withCentre (displayComboArea.getCentre()));
        area.removeFromRight (gap);
        auto displayLblArea = area.removeFromRight (lblW);
        displayLabel.setBounds (displayLblArea.withHeight (22).withCentre (displayLblArea.getCentre()));
        area.removeFromRight (gap * 2);

        // 6 Knobs (5 timing/link + output) centered in remaining space
        int totalKnobW = knobW * 6;
        auto knobRow = area.withSizeKeepingCentre (totalKnobW, area.getHeight());

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
        layoutKnob (outputLevelKnob, outputLevelLabel, outputLevelValue, knobRow, knobW);
    }

    // === Row 2 (merged): Toggle buttons left | Dither + combos right ===
    {
        auto area = bottomBar.reduced (8, 2);
        int btnH = 24;
        int btnW = 36;
        int gap = 6;
        int comboH = 22;
        int comboW = 58;
        int ditherBtnW = 44;
        int lblW = 36;

        // === Left side: toggle buttons ===
        auto leftSide = area.removeFromLeft (area.getWidth() / 2);
        {
            auto btnArea = leftSide.withHeight (btnH).withY (
                leftSide.getCentreY() - btnH / 2);

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

        // === Right side: Dither | Bits | Shape | OS | Meter ===
        auto rightSide = area;
        {
            // Meter Scale combo on far right
            auto mArea = rightSide.removeFromRight (comboW);
            meterScaleCombo.setBounds (mArea.withHeight (comboH).withCentre (mArea.getCentre()));
            auto mLbl = rightSide.removeFromRight (lblW);
            meterScaleLabel.setBounds (mLbl.withHeight (comboH).withCentre (mLbl.getCentre()));
            rightSide.removeFromRight (gap);

            // OS combo
            auto osArea = rightSide.removeFromRight (comboW);
            oversampleCombo.setBounds (osArea.withHeight (comboH).withCentre (osArea.getCentre()));
            auto osLbl = rightSide.removeFromRight (lblW);
            oversampleLabel.setBounds (osLbl.withHeight (comboH).withCentre (osLbl.getCentre()));
            rightSide.removeFromRight (gap);

            // Noise Shape
            auto shapeArea = rightSide.removeFromRight (comboW);
            noiseShapeCombo.setBounds (shapeArea.withHeight (comboH).withCentre (shapeArea.getCentre()));
            auto shapeLbl = rightSide.removeFromRight (lblW);
            noiseShapeLabel.setBounds (shapeLbl.withHeight (comboH).withCentre (shapeLbl.getCentre()));
            rightSide.removeFromRight (gap);

            // Dither bits
            auto bitsArea = rightSide.removeFromRight (comboW);
            ditherBitsCombo.setBounds (bitsArea.withHeight (comboH).withCentre (bitsArea.getCentre()));
            auto bitsLbl = rightSide.removeFromRight (lblW);
            ditherBitsLabel.setBounds (bitsLbl.withHeight (comboH).withCentre (bitsLbl.getCentre()));
            rightSide.removeFromRight (gap);

            // Dither toggle
            auto ditherArea = rightSide.removeFromRight (ditherBtnW);
            ditherOnButton.setBounds (ditherArea.withHeight (comboH).withCentre (ditherArea.getCentre()));
        }
    }
}

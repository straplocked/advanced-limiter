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

#include "PluginEditor.h"
#include "utils/Constants.h"
#include "ui/LookAndFeel/ColourPalette.h"
#include "parameters/ParameterIDs.h"

AdvancedLimiterEditor::AdvancedLimiterEditor (AdvancedLimiterProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p),
      headerToolbar (p),
      limiterDisplay (p.getLimiterEngine().getMeterData()),
      levelMeter (p.getLimiterEngine().getMeterData()),
      loudnessPanel (p.getLimiterEngine().getMeterData()),
      inputGainStrip (p.getAPVTS()),
      controlStrip (p.getAPVTS()),
      presetBrowser (p.getPresetManager())
{
    setLookAndFeel (&lookAndFeel);

    addAndMakeVisible (headerToolbar);
    addAndMakeVisible (limiterDisplay);
    addAndMakeVisible (levelMeter);
    addAndMakeVisible (loudnessPanel);
    addAndMakeVisible (inputGainStrip);
    addAndMakeVisible (controlStrip);

    // Preset browser: hidden initially
    addChildComponent (presetBrowser);

    // Help overlay always on top
    addAndMakeVisible (helpOverlay);

    // Wire up callbacks
    headerToolbar.onPresetNameClicked = [this]
    {
        presetBrowser.setVisible (! presetBrowser.isVisible());
    };

    loudnessPanel.onResetClicked = [&p]
    {
        p.getLimiterEngine().getMeterData().resetLoudness();
    };

    // Listen for display parameter changes
    auto& apvts = p.getAPVTS();
    apvts.addParameterListener (ParamIDs::kDisplayMode, this);
    apvts.addParameterListener (ParamIDs::kMeterScale, this);
    apvts.addParameterListener (ParamIDs::kLoudnessTarget, this);

    // Apply initial values
    limiterDisplay.setDisplayMode ((int) *apvts.getRawParameterValue (ParamIDs::kDisplayMode));
    limiterDisplay.setMeterScale ((int) *apvts.getRawParameterValue (ParamIDs::kMeterScale));
    levelMeter.setMeterScale ((int) *apvts.getRawParameterValue (ParamIDs::kMeterScale));
    loudnessPanel.setLoudnessTarget ((int) *apvts.getRawParameterValue (ParamIDs::kLoudnessTarget));

    setSize (Constants::kDefaultEditorWidth, Constants::kDefaultEditorHeight);
    setResizable (true, true);
    setResizeLimits (946, 545, 1980, 1485);
}

AdvancedLimiterEditor::~AdvancedLimiterEditor()
{
    auto& apvts = processorRef.getAPVTS();
    apvts.removeParameterListener (ParamIDs::kDisplayMode, this);
    apvts.removeParameterListener (ParamIDs::kMeterScale, this);
    apvts.removeParameterListener (ParamIDs::kLoudnessTarget, this);
    setLookAndFeel (nullptr);
}

void AdvancedLimiterEditor::parameterChanged (const juce::String& parameterID, float newValue)
{
    int val = (int) newValue;
    if (parameterID == ParamIDs::kDisplayMode)
        limiterDisplay.setDisplayMode (val);
    else if (parameterID == ParamIDs::kMeterScale)
    {
        limiterDisplay.setMeterScale (val);
        levelMeter.setMeterScale (val);
    }
    else if (parameterID == ParamIDs::kLoudnessTarget)
        loudnessPanel.setLoudnessTarget (val);
}

void AdvancedLimiterEditor::paint (juce::Graphics& g)
{
    g.fillAll (ColourPalette::background());
}

void AdvancedLimiterEditor::resized()
{
    auto bounds = getLocalBounds();

    // Top: header toolbar
    headerToolbar.setBounds (bounds.removeFromTop (Constants::kHeaderHeight));

    // Bottom: control strip
    controlStrip.setBounds (bounds.removeFromBottom (Constants::kBottomStripHeight));

    // Right: level meter
    levelMeter.setBounds (bounds.removeFromRight (Constants::kMeterWidth));

    // Left: input gain strip
    inputGainStrip.setBounds (bounds.removeFromLeft (Constants::kInputGainStripWidth));

    // Main display area (center)
    limiterDisplay.setBounds (bounds);

    // Loudness panel overlaid at bottom of display area
    auto displayBounds = limiterDisplay.getBounds();
    loudnessPanel.setBounds (displayBounds.getX(),
                              displayBounds.getBottom() - Constants::kLoudnessPanelHeight,
                              displayBounds.getWidth(), Constants::kLoudnessPanelHeight);

    // Preset browser overlaid on right side of display
    presetBrowser.setBounds (displayBounds.getRight() - 385,
                              displayBounds.getY(),
                              385, displayBounds.getHeight());

    // Help overlay covers everything
    helpOverlay.setBounds (getLocalBounds());
    helpOverlay.toFront (false);
}

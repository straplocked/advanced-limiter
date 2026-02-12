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

AdvancedLimiterEditor::AdvancedLimiterEditor (AdvancedLimiterProcessor& p)
    : AudioProcessorEditor (&p),
      headerToolbar (p),
      limiterDisplay (p.getLimiterEngine().getMeterData()),
      levelMeter (p.getLimiterEngine().getMeterData()),
      loudnessPanel (p.getLimiterEngine().getMeterData()),
      controlTray (p.getAPVTS()),
      controlStrip (p.getAPVTS()),
      presetBrowser (p.getPresetManager())
{
    setLookAndFeel (&lookAndFeel);

    addAndMakeVisible (headerToolbar);
    addAndMakeVisible (limiterDisplay);
    addAndMakeVisible (levelMeter);
    addAndMakeVisible (loudnessPanel);
    addAndMakeVisible (controlStrip);

    // Control tray: hidden initially
    addChildComponent (controlTray);

    // Preset browser: hidden initially
    addChildComponent (presetBrowser);

    // Help overlay always on top
    addAndMakeVisible (helpOverlay);

    // Wire up callbacks
    controlStrip.onControlsToggle = [this] { toggleControlTray(); };

    headerToolbar.onPresetNameClicked = [this]
    {
        presetBrowser.setVisible (! presetBrowser.isVisible());
    };

    loudnessPanel.onResetClicked = [&p]
    {
        p.getLimiterEngine().getMeterData().resetLoudness();
    };

    setSize (Constants::kDefaultEditorWidth, Constants::kDefaultEditorHeight);
    setResizable (true, true);
    setResizeLimits (860, 495, 1800, 1350);
}

AdvancedLimiterEditor::~AdvancedLimiterEditor()
{
    setLookAndFeel (nullptr);
}

void AdvancedLimiterEditor::paint (juce::Graphics& g)
{
    g.fillAll (ColourPalette::background());
}

void AdvancedLimiterEditor::resized()
{
    auto bounds = getLocalBounds();

    // Top: header toolbar (36 px)
    headerToolbar.setBounds (bounds.removeFromTop (36));

    // Bottom: control strip (80 px)
    int controlStripH = 80;
    controlStrip.setBounds (bounds.removeFromBottom (controlStripH));

    // If control tray is open, it sits above the control strip
    int controlTrayH = 140;
    if (controlTrayOpen)
    {
        controlTray.setBounds (bounds.removeFromBottom (controlTrayH));
    }
    else
    {
        // Position offscreen for animation target
        controlTray.setBounds (bounds.getX(), bounds.getBottom(),
                               bounds.getWidth(), controlTrayH);
    }

    // Remaining area: split between display (main) and level meter (right strip)
    int meterWidth = 100;
    levelMeter.setBounds (bounds.removeFromRight (meterWidth));

    // Main display area
    limiterDisplay.setBounds (bounds);

    // Loudness panel overlaid at bottom of display area (30 px tall)
    auto displayBounds = limiterDisplay.getBounds();
    loudnessPanel.setBounds (displayBounds.getX(), displayBounds.getBottom() - 30,
                              displayBounds.getWidth(), 30);

    // Preset browser overlaid on right side of display
    presetBrowser.setBounds (displayBounds.getRight() - 350,
                              displayBounds.getY(),
                              350, displayBounds.getHeight());

    // Help overlay covers everything
    helpOverlay.setBounds (getLocalBounds());
    helpOverlay.toFront (false);
}

void AdvancedLimiterEditor::toggleControlTray()
{
    controlTrayOpen = ! controlTrayOpen;
    controlStrip.setControlTrayOpen (controlTrayOpen);

    if (controlTrayOpen)
    {
        controlTray.setVisible (true);

        // Animate into position
        auto targetBounds = controlTray.getBounds();
        auto bounds = getLocalBounds();
        int controlStripH = 80;
        int controlTrayH = 140;

        // Calculate final position
        int trayY = bounds.getHeight() - 36 - controlStripH - controlTrayH;
        // Actually, we need to just re-layout
        resized();
    }
    else
    {
        // Hide after brief delay
        controlTray.setVisible (false);
        resized();
    }
}

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

#include "PluginProcessor.h"
#include "ui/components/LimiterDisplay.h"
#include "ui/components/LevelMeterComponent.h"
#include "ui/components/LoudnessPanel.h"
#include "ui/components/ControlTray.h"
#include "ui/components/BottomControlStrip.h"
#include "ui/components/HeaderToolbar.h"
#include "ui/components/PresetBrowserPanel.h"
#include "ui/help/HelpOverlay.h"
#include "ui/LookAndFeel/AdvancedLimiterLookAndFeel.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>

class AdvancedLimiterEditor : public juce::AudioProcessorEditor,
                              public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit AdvancedLimiterEditor (AdvancedLimiterProcessor&);
    ~AdvancedLimiterEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void parameterChanged (const juce::String& parameterID, float newValue) override;

private:
    AdvancedLimiterProcessor& processorRef;
    AdvancedLimiterLookAndFeel lookAndFeel;

    HeaderToolbar headerToolbar;
    LimiterDisplay limiterDisplay;
    LevelMeterComponent levelMeter;
    LoudnessPanel loudnessPanel;
    ControlTray controlTray;
    BottomControlStrip controlStrip;
    PresetBrowserPanel presetBrowser;
    HelpOverlay helpOverlay;

    bool controlTrayOpen = false;
    juce::ComponentAnimator animator;

    void toggleControlTray();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdvancedLimiterEditor)
};

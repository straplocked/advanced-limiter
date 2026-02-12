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
#include "../../presets/PresetManager.h"
#include "../LookAndFeel/ColourPalette.h"

class AdvancedLimiterProcessor;

class HeaderToolbar : public juce::Component,
                      public PresetManager::Listener
{
public:
    explicit HeaderToolbar (AdvancedLimiterProcessor& processor);
    ~HeaderToolbar() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void presetChanged() override;

    std::function<void()> onPresetNameClicked;

private:
    AdvancedLimiterProcessor& processor;
    PresetManager& presetManager;

    // Preset controls (Unicode icon buttons)
    juce::TextButton prevButton { juce::String::charToString (0x25C0) };
    juce::TextButton nextButton { juce::String::charToString (0x25B6) };
    juce::TextButton presetNameButton { "Init" };
    juce::TextButton saveButton { juce::String::charToString (0x2913) };

    // Actions (Unicode icon buttons)
    juce::TextButton undoButton { juce::String::charToString (0x21A9) };
    juce::TextButton redoButton { juce::String::charToString (0x21AA) };

    // Text buttons
    juce::TextButton deltaButton { "Delta" };
    juce::TextButton abButton { "A" };
    juce::TextButton copyABButton { "A>B" };
    juce::TextButton midiLearnButton { "MIDI" };

    // APVTS attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> deltaAttachment;

    void showPresetMenu();
    void showSaveDialog();
    void updatePresetName();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderToolbar)
};

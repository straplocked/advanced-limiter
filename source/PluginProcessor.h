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

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "dsp/LimiterEngine.h"
#include "parameters/LimiterParameterAttachment.h"
#include "presets/PresetManager.h"
#include "midi/MIDILearnManager.h"
#include "utils/Constants.h"

class AdvancedLimiterProcessor : public juce::AudioProcessor
{
public:
    AdvancedLimiterProcessor();
    ~AdvancedLimiterProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override;
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    LimiterEngine& getLimiterEngine() { return limiterEngine; }
    PresetManager& getPresetManager() { return presetManager; }
    LimiterParameterAttachment& getParameterAttachment() { return paramAttachment; }
    juce::UndoManager& getUndoManager() { return undoManager; }
    MIDILearnManager& getMIDILearnManager() { return midiLearnManager; }

    bool isStateA() const { return currentIsA; }
    void toggleAB();
    void copyAtoB();
    void copyBtoA();

private:
    juce::UndoManager undoManager { 30000, 30 };
    juce::AudioProcessorValueTreeState apvts;
    PresetManager presetManager;
    LimiterEngine limiterEngine;
    LimiterParameterAttachment paramAttachment;
    MIDILearnManager midiLearnManager;

    bool currentIsA = true;
    juce::ValueTree stateA, stateB;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdvancedLimiterProcessor)
};

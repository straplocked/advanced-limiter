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

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "parameters/ParameterLayout.h"
#include "parameters/ParameterIDs.h"

AdvancedLimiterProcessor::AdvancedLimiterProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                      .withInput  ("Sidechain", juce::AudioChannelSet::stereo(), false)),
      apvts (*this, &undoManager, "AdvancedLimiterState", ParameterLayout::createLayout()),
      presetManager (apvts),
      midiLearnManager (apvts)
{
    paramAttachment.init (apvts);
}

AdvancedLimiterProcessor::~AdvancedLimiterProcessor() = default;

const juce::String AdvancedLimiterProcessor::getName() const
{
    return JucePlugin_Name;
}

void AdvancedLimiterProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    limiterEngine.prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void AdvancedLimiterProcessor::releaseResources()
{
    limiterEngine.reset();
}

bool AdvancedLimiterProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    auto mainOutput = layouts.getMainOutputChannelSet();
    auto mainInput = layouts.getMainInputChannelSet();

    if (mainOutput != juce::AudioChannelSet::mono()
        && mainOutput != juce::AudioChannelSet::stereo())
        return false;

    if (mainOutput != mainInput)
        return false;

    // Sidechain can be disabled or stereo
    if (layouts.inputBuses.size() > 1)
    {
        auto sc = layouts.getChannelSet (true, 1);
        if (! sc.isDisabled() && sc != juce::AudioChannelSet::mono()
            && sc != juce::AudioChannelSet::stereo())
            return false;
    }

    return true;
}

void AdvancedLimiterProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Read parameters
    auto params = paramAttachment.getCurrentParameters();
    limiterEngine.updateParameters (params);

    // Get sidechain buffer if available
    const juce::AudioBuffer<float>* sidechainBuffer = nullptr;
    juce::AudioBuffer<float> scBuf;
    if (params.scExternal && getBus (true, 1) != nullptr && getBus (true, 1)->isEnabled())
    {
        auto scChannels = getBus (true, 1)->getNumberOfChannels();
        if (scChannels > 0)
        {
            int mainChannels = getBus (true, 0)->getNumberOfChannels();
            scBuf.setSize (scChannels, buffer.getNumSamples(), false, false, true);
            for (int ch = 0; ch < scChannels; ++ch)
            {
                if (mainChannels + ch < buffer.getNumChannels())
                    scBuf.copyFrom (ch, 0, buffer, mainChannels + ch, 0, buffer.getNumSamples());
            }
            sidechainBuffer = &scBuf;
        }
    }

    // Process MIDI learn CC mappings
    midiLearnManager.processMidi (midiMessages);

    // Process limiter
    limiterEngine.process (buffer);

    // Report latency
    setLatencySamples (limiterEngine.getLatencySamples());
}

juce::AudioProcessorEditor* AdvancedLimiterProcessor::createEditor()
{
    return new AdvancedLimiterEditor (*this);
}

void AdvancedLimiterProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    midiLearnManager.saveToState (state);
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AdvancedLimiterProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
    {
        auto state = juce::ValueTree::fromXml (*xml);
        midiLearnManager.loadFromState (state);
        apvts.replaceState (state);
    }
}

void AdvancedLimiterProcessor::toggleAB()
{
    if (currentIsA)
    {
        stateA = apvts.copyState();
        if (stateB.isValid())
            apvts.replaceState (stateB);
    }
    else
    {
        stateB = apvts.copyState();
        if (stateA.isValid())
            apvts.replaceState (stateA);
    }
    currentIsA = ! currentIsA;
}

void AdvancedLimiterProcessor::copyAtoB()
{
    if (currentIsA)
        stateB = apvts.copyState();
    else
    {
        stateB = stateA.createCopy();
        apvts.replaceState (stateB);
    }
}

void AdvancedLimiterProcessor::copyBtoA()
{
    if (! currentIsA)
        stateA = apvts.copyState();
    else
    {
        stateA = stateB.createCopy();
        apvts.replaceState (stateA);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AdvancedLimiterProcessor();
}

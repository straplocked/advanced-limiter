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

#include "LoudnessPanel.h"
#include <cmath>

LoudnessPanel::LoudnessPanel (LevelMeterData& md)
    : meterData (md)
{
    resetButton.setComponentID ("loudness.reset");
    resetButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    resetButton.setColour (juce::TextButton::textColourOffId, ColourPalette::textDim());
    resetButton.onClick = [this] { if (onResetClicked) onResetClicked(); };
    addAndMakeVisible (resetButton);

    startTimerHz (10);
}

LoudnessPanel::~LoudnessPanel()
{
    stopTimer();
}

void LoudnessPanel::timerCallback()
{
    auto& buffer = meterData.getBuffer();
    if (buffer.swapRead())
    {
        const auto* frame = buffer.getReadPointer();
        momentary = frame->momentaryLUFS;
        shortTerm = frame->shortTermLUFS;
        integrated = frame->integratedLUFS;
        lra = frame->lra;
    }
    repaint();
}

void LoudnessPanel::setLoudnessTarget (int target)
{
    loudnessTarget = target;
    repaint();
}

void LoudnessPanel::setLoudnessValues (float m, float s, float i, float range)
{
    momentary = m;
    shortTerm = s;
    integrated = i;
    lra = range;
}

void LoudnessPanel::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Semi-transparent background
    g.setColour (ColourPalette::controlBar().withAlpha (0.85f));
    g.fillRect (bounds);

    // Top bevel
    g.setColour (ColourPalette::bevelLight().withAlpha (0.5f));
    g.drawLine (bounds.getX(), bounds.getY(), bounds.getRight(), bounds.getY(), 0.5f);

    // Loudness readout
    float xPos = 10.0f;
    float textH = bounds.getHeight();

    auto drawLoudnessValue = [&] (const juce::String& label, float value,
                                   const juce::Colour& labelColour, bool isLUFS)
    {
        g.setFont (Fonts::getBody (10.0f));
        g.setColour (labelColour);
        g.drawText (label, xPos, 0.0f, 20.0f, textH, juce::Justification::centredLeft);
        xPos += 20.0f;

        g.setFont (Fonts::getBold (11.0f));
        g.setColour (ColourPalette::textBright());

        juce::String valStr;
        if (value <= -99.0f)
            valStr = "---";
        else
            valStr = juce::String (value, 1) + (isLUFS ? "" : " LU");

        g.drawText (valStr, xPos, 0.0f, 50.0f, textH, juce::Justification::centredLeft);
        xPos += 56.0f;
    };

    drawLoudnessValue ("M:", momentary, ColourPalette::outputLevel(), true);
    drawLoudnessValue ("S:", shortTerm, ColourPalette::loudnessTarget(), true);
    drawLoudnessValue ("I:", integrated, ColourPalette::gainReduction(), true);

    // LRA
    g.setFont (Fonts::getBody (10.0f));
    g.setColour (ColourPalette::axisText());
    g.drawText ("LRA:", xPos, 0.0f, 30.0f, textH, juce::Justification::centredLeft);
    xPos += 30.0f;

    g.setFont (Fonts::getBold (11.0f));
    g.setColour (ColourPalette::textBright());
    juce::String lraStr = (lra > 0.01f) ? juce::String (lra, 1) : "---";
    g.drawText (lraStr, xPos, 0.0f, 40.0f, textH, juce::Justification::centredLeft);
    xPos += 46.0f;

    // Loudness target indicator
    if (loudnessTarget > 0)
    {
        float targetLUFS = 0.0f;
        switch (loudnessTarget)
        {
            case 1: targetLUFS = -23.0f; break;
            case 2: targetLUFS = -16.0f; break;
            case 3: targetLUFS = -14.0f; break;
            default: break;
        }

        g.setFont (Fonts::getBody (9.0f));
        g.setColour (ColourPalette::loudnessTarget());

        juce::String targetStr = "Target: " + juce::String ((int) targetLUFS) + " LUFS";
        g.drawText (targetStr, xPos, 0.0f, 110.0f, textH, juce::Justification::centredLeft);

        // Draw small colored indicator showing if we're above/below target
        if (integrated > -99.0f)
        {
            float diff = integrated - targetLUFS;
            juce::Colour indicatorColour = (std::abs (diff) < 1.0f)
                ? ColourPalette::meterGreen()
                : (diff > 0.0f ? ColourPalette::meterRed() : ColourPalette::loudnessTarget());
            g.setColour (indicatorColour);
            g.fillEllipse (xPos + 112.0f, textH * 0.5f - 3.0f, 6.0f, 6.0f);
        }
    }
}

void LoudnessPanel::resized()
{
    auto bounds = getLocalBounds();
    int btnW = 28;
    resetButton.setBounds (bounds.removeFromRight (btnW).reduced (2, 4));
}

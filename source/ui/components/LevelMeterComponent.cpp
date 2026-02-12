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

#include "LevelMeterComponent.h"
#include "../../utils/Constants.h"
#include <cmath>
#include <algorithm>

LevelMeterComponent::LevelMeterComponent (LevelMeterData& md)
    : meterData (md)
{
    // Initialize peak hold calculators at 30 Hz UI rate
    peakHoldInputL.prepare (30.0);
    peakHoldInputR.prepare (30.0);
    peakHoldOutputL.prepare (30.0);
    peakHoldOutputR.prepare (30.0);

    startTimerHz (Constants::kUITimerHz);
}

LevelMeterComponent::~LevelMeterComponent()
{
    stopTimer();
}

void LevelMeterComponent::timerCallback()
{
    // Read from the cached frame (LimiterDisplay is the sole TripleBuffer consumer)
    const auto& frame = meterData.getLatestFrame();

    // Smooth with 0.8 decay
    constexpr float decay = 0.8f;
    constexpr float attack = 0.4f;

    auto smooth = [] (float& current, float target, float atk, float dec)
    {
        float coeff = (target > current) ? atk : dec;
        current = current + coeff * (target - current);
    };

    smooth (smoothInputL, frame.inputL, attack, decay);
    smooth (smoothInputR, frame.inputR, attack, decay);
    smooth (smoothOutputL, frame.outputL, attack, decay);
    smooth (smoothOutputR, frame.outputR, attack, decay);

    float gr = std::min (frame.grL, frame.grR);
    smooth (smoothGR, gr, attack, decay);

    truePeakL = frame.truePeakL;
    truePeakR = frame.truePeakR;

    // Feed peak hold calculators
    peakHoldInputL.process (smoothInputL);
    peakHoldInputR.process (smoothInputR);
    peakHoldOutputL.process (smoothOutputL);
    peakHoldOutputR.process (smoothOutputR);

    repaint();
}

void LevelMeterComponent::setMeterScale (int scale) { meterScale = scale; }

float LevelMeterComponent::getScaleRange() const
{
    if (meterScale >= 0 && meterScale < 6)
        return Constants::kMeterScaleRanges[meterScale];
    return 32.0f;
}

float LevelMeterComponent::dbToY (float db, float height) const
{
    float scaleRange = getScaleRange();
    float topDB = 6.0f;
    float bottomDB = topDB - scaleRange;
    float normalized = (db - bottomDB) / (topDB - bottomDB);
    normalized = juce::jlimit (0.0f, 1.0f, normalized);
    return height * (1.0f - normalized);
}

void LevelMeterComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour (ColourPalette::curveArea());
    g.fillRoundedRectangle (bounds, 3.0f);

    float labelHeight = 14.0f;
    auto meterArea = bounds.reduced (3.0f, 2.0f);
    meterArea.removeFromTop (labelHeight);
    meterArea.removeFromBottom (labelHeight);

    float totalWidth = meterArea.getWidth();
    float gap = 2.0f;
    // 5 bars: InL, InR, gap, OutL, OutR, gap, GR
    float barW = (totalWidth - gap * 2.0f) / 5.0f;

    float x = meterArea.getX();
    float h = meterArea.getHeight();
    float y = meterArea.getY();

    // Input L
    auto inLArea = juce::Rectangle<float> (x, y, barW, h);
    drawMeterBar (g, inLArea, smoothInputL, peakHoldInputL.process (smoothInputL), false);
    x += barW;

    // Input R
    auto inRArea = juce::Rectangle<float> (x, y, barW, h);
    drawMeterBar (g, inRArea, smoothInputR, peakHoldInputR.process (smoothInputR), false);
    x += barW + gap;

    // Output L
    auto outLArea = juce::Rectangle<float> (x, y, barW, h);
    drawMeterBar (g, outLArea, smoothOutputL, peakHoldOutputL.process (smoothOutputL), false);
    x += barW;

    // Output R
    auto outRArea = juce::Rectangle<float> (x, y, barW, h);
    drawMeterBar (g, outRArea, smoothOutputR, peakHoldOutputR.process (smoothOutputR), false);
    x += barW + gap;

    // True peak markers on output bars
    drawTruePeakMarker (g, outLArea, truePeakL);
    drawTruePeakMarker (g, outRArea, truePeakR);

    // GR bar (inverted)
    auto grArea = juce::Rectangle<float> (x, y, barW, h);
    drawMeterBar (g, grArea, smoothGR, smoothGR, true);

    // Labels
    g.setFont (Fonts::getBody (9.0f));
    g.setColour (ColourPalette::axisText());

    float labelY = bounds.getY() + 1.0f;
    float meterLeft = meterArea.getX();
    g.drawText ("In", meterLeft, labelY, barW * 2.0f, labelHeight, juce::Justification::centred);
    g.drawText ("Out", meterLeft + barW * 2.0f + gap, labelY, barW * 2.0f, labelHeight, juce::Justification::centred);
    g.drawText ("GR", meterLeft + barW * 4.0f + gap * 2.0f, labelY, barW, labelHeight, juce::Justification::centred);

    // dB scale labels on the right edge
    float scaleRange = getScaleRange();
    float topDB = 6.0f;
    float stepDB = (scaleRange <= 16.0f) ? 3.0f : 6.0f;
    g.setFont (Fonts::getBody (8.0f));

    for (float db = topDB; db >= topDB - scaleRange; db -= stepDB)
    {
        float meterY = dbToY (db, h) + meterArea.getY();
        g.setColour (ColourPalette::gridLine());
        g.drawLine (meterArea.getX(), meterY, meterArea.getRight(), meterY, 0.3f);
    }

    // 0 dB reference line
    float zeroY = dbToY (0.0f, h) + meterArea.getY();
    g.setColour (ColourPalette::ceilingLine().withAlpha (0.3f));
    g.drawLine (meterArea.getX(), zeroY, meterArea.getRight(), zeroY, 0.5f);

    // Bottom value readout
    float readoutY = meterArea.getBottom() + 1.0f;
    g.setFont (Fonts::getBody (9.0f));
    g.setColour (ColourPalette::gainReduction());
    juce::String grText = (smoothGR < -0.1f) ? juce::String (smoothGR, 1) : "0.0";
    g.drawText (grText, bounds.getX(), readoutY, bounds.getWidth(), labelHeight,
                juce::Justification::centred);

    // Border
    g.setColour (ColourPalette::gridLine());
    g.drawRoundedRectangle (bounds, 3.0f, 0.5f);
}

void LevelMeterComponent::resized()
{
    // No child components
}

void LevelMeterComponent::drawMeterBar (juce::Graphics& g, juce::Rectangle<float> area,
                                         float valueDB, float peakDB, bool isGR)
{
    float h = area.getHeight();
    float x = area.getX();
    float y = area.getY();
    float w = area.getWidth();

    if (isGR)
    {
        // GR meter: fill from top (0 dB) downward
        if (valueDB < -0.01f)
        {
            float zeroY = dbToY (0.0f, h);
            float grY = dbToY (valueDB, h);

            g.setColour (ColourPalette::gainReduction().withAlpha (0.7f));
            g.fillRect (x, y + zeroY, w, grY - zeroY);

            // Orange bottom edge
            g.setColour (ColourPalette::gainReduction());
            g.fillRect (x, y + grY - 1.0f, w, 1.0f);
        }
    }
    else
    {
        // Normal meter: gradient bar from bottom up
        float meterY = dbToY (valueDB, h);

        juce::ColourGradient gradient (ColourPalette::meterGreen(), x, y + h,
                                       ColourPalette::meterRed(), x, y, false);
        gradient.addColour (0.6, ColourPalette::meterYellow());
        g.setGradientFill (gradient);
        g.fillRect (x, y + meterY, w, h - meterY);

        // Peak hold line
        if (peakDB > Constants::kMeterMinDB + 1.0f)
        {
            float peakY = dbToY (peakDB, h);
            g.setColour (ColourPalette::textBright());
            g.fillRect (x, y + peakY, w, 1.0f);
        }
    }
}

void LevelMeterComponent::drawTruePeakMarker (juce::Graphics& g, juce::Rectangle<float> area,
                                                float tpDB)
{
    if (tpDB < Constants::kMeterMinDB + 1.0f)
        return;

    float h = area.getHeight();
    float tpY = dbToY (tpDB, h) + area.getY();

    // Small red triangle marker
    float triSize = 4.0f;
    juce::Path tri;
    tri.addTriangle (area.getRight() - triSize, tpY - triSize * 0.5f,
                     area.getRight() - triSize, tpY + triSize * 0.5f,
                     area.getRight(), tpY);
    g.setColour (ColourPalette::truePeakMarker());
    g.fillPath (tri);
}

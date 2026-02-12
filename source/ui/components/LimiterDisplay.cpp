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

#include "LimiterDisplay.h"
#include "../../utils/Constants.h"
#include <cmath>
#include <algorithm>

LimiterDisplay::LimiterDisplay (LevelMeterData& md)
    : meterData (md)
{
    history.prepare (Constants::kScrollingHistorySize);
    startTimerHz (Constants::kUITimerHz);
}

LimiterDisplay::~LimiterDisplay()
{
    stopTimer();
}

void LimiterDisplay::timerCallback()
{
    if (displayMode == 4) // Off
        return;

    auto& buffer = meterData.getBuffer();
    if (buffer.swapRead())
    {
        const auto* frame = buffer.getReadPointer();
        ScrollingHistoryPoint pt;
        pt.inputLevel = std::max (frame->inputL, frame->inputR);
        pt.outputLevel = std::max (frame->outputL, frame->outputR);
        pt.gainReduction = std::min (frame->grL, frame->grR);
        history.push (pt);

        repaint();
    }
}

void LimiterDisplay::setDisplayMode (int mode) { displayMode = mode; }
void LimiterDisplay::setMeterScale (int scale) { meterScale = scale; }

float LimiterDisplay::getScaleRange() const
{
    if (meterScale >= 0 && meterScale < 6)
        return Constants::kMeterScaleRanges[meterScale];
    return 32.0f;
}

void LimiterDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background
    g.setColour (ColourPalette::curveArea());
    g.fillRect (bounds);

    // Inset border
    g.setColour (ColourPalette::screenShadow());
    g.drawRect (bounds, 1.0f);
    g.setColour (ColourPalette::screenHighlight());
    g.drawLine (bounds.getX() + 1.0f, bounds.getBottom() - 0.5f,
                bounds.getRight() - 1.0f, bounds.getBottom() - 0.5f, 0.5f);

    // Draw area with left margin for labels
    float leftMargin = 36.0f;
    auto drawArea = bounds.withTrimmedLeft (leftMargin).reduced (1.0f, 1.0f);

    drawGrid (g, drawArea);

    if (displayMode != 4)
    {
        drawScrollingData (g, drawArea);
        drawGRPeakLabels (g, drawArea);
    }
}

void LimiterDisplay::resized()
{
    // No child components to layout
}

void LimiterDisplay::drawGrid (juce::Graphics& g, juce::Rectangle<float> area)
{
    float scaleRange = getScaleRange();
    float topDB = 0.0f;
    float bottomDB = -scaleRange;

    g.setFont (Fonts::getBody (9.0f));

    // Draw horizontal lines at every 6 dB (or 3 dB for small scales)
    float stepDB = (scaleRange <= 16.0f) ? 3.0f : 6.0f;

    for (float db = topDB; db >= bottomDB; db -= stepDB)
    {
        float normalized = (db - bottomDB) / (topDB - bottomDB);
        float y = area.getBottom() - normalized * area.getHeight();

        bool isStrong = (std::fmod (std::abs (db), 12.0f) < 0.1f) || (db == 0.0f);
        g.setColour (isStrong ? ColourPalette::gridLineStrong() : ColourPalette::gridLine());
        g.drawLine (area.getX(), y, area.getRight(), y, 0.5f);

        // Label on left side (in margin)
        g.setColour (ColourPalette::axisText());
        juce::String label = (db == 0.0f) ? "0" : juce::String ((int) db);
        g.drawText (label, area.getX() - 34.0f, y - 6.0f, 30.0f, 12.0f,
                    juce::Justification::centredRight);
    }

    // 0 dB line (ceiling reference) drawn in red
    float zeroY = area.getY();
    g.setColour (ColourPalette::ceilingLine().withAlpha (0.4f));
    g.drawLine (area.getX(), zeroY, area.getRight(), zeroY, 1.0f);
}

void LimiterDisplay::drawScrollingData (juce::Graphics& g, juce::Rectangle<float> area)
{
    float scaleRange = getScaleRange();
    float topDB = 0.0f;
    float bottomDB = -scaleRange;

    int pointsToShow = juce::jmin (history.getSize(), (int) area.getWidth());
    if (pointsToShow <= 0)
        return;

    // Decimation factor based on display mode
    int decimation = 1;
    switch (displayMode)
    {
        case 0: decimation = 4; break;  // SlowDown
        case 1: decimation = 1; break;  // Fast
        case 2: decimation = 2; break;  // Slow
        case 3: decimation = 1; break;  // Infinite (no decay)
        default: break;
    }

    float pixelWidth = 1.0f;

    auto dbToY = [&] (float db) -> float
    {
        float clamped = juce::jlimit (bottomDB, topDB, db);
        float normalized = (clamped - bottomDB) / (topDB - bottomDB);
        return area.getBottom() - normalized * area.getHeight();
    };

    // Draw from right to left
    for (int px = 0; px < pointsToShow; ++px)
    {
        int histIdx = px * decimation;
        if (histIdx >= history.getSize())
            break;

        auto pt = history.getPointFromEnd (histIdx);
        float x = area.getRight() - (float) px - 1.0f;

        if (x < area.getX())
            break;

        float zeroY = dbToY (0.0f);

        // Input level: green fill from bottom upward
        if (pt.inputLevel > bottomDB)
        {
            float inY = dbToY (pt.inputLevel);
            g.setColour (ColourPalette::inputLevel().withAlpha (0.3f));
            g.fillRect (x, inY, pixelWidth, area.getBottom() - inY);
        }

        // Output level: lime fill overlaid
        if (pt.outputLevel > bottomDB)
        {
            float outY = dbToY (pt.outputLevel);
            g.setColour (ColourPalette::outputLevel().withAlpha (0.5f));
            g.fillRect (x, outY, pixelWidth, area.getBottom() - outY);
        }

        // Gain reduction: orange fill from 0 dB downward
        if (pt.gainReduction < -0.01f)
        {
            float grY = dbToY (pt.gainReduction);
            float topY = zeroY;
            if (grY > topY)
            {
                g.setColour (ColourPalette::gainReductionFill());
                g.fillRect (x, topY, pixelWidth, grY - topY);

                // Orange line at bottom of GR
                g.setColour (ColourPalette::gainReduction());
                g.fillRect (x, grY - 0.5f, pixelWidth, 1.0f);
            }
        }
    }
}

void LimiterDisplay::drawGRPeakLabels (juce::Graphics& g, juce::Rectangle<float> area)
{
    // Find the peak GR in the visible history
    float scaleRange = getScaleRange();
    float topDB = 0.0f;
    float bottomDB = -scaleRange;
    int pointsToShow = juce::jmin (history.getSize(), (int) area.getWidth());

    float peakGR = 0.0f;
    int peakIdx = 0;

    for (int i = 0; i < pointsToShow; ++i)
    {
        auto pt = history.getPointFromEnd (i);
        if (pt.gainReduction < peakGR)
        {
            peakGR = pt.gainReduction;
            peakIdx = i;
        }
    }

    if (peakGR > -0.1f)
        return; // No significant GR

    // Position label at peak point
    float x = area.getRight() - (float) peakIdx - 1.0f;
    float normalized = (peakGR - bottomDB) / (topDB - bottomDB);
    float y = area.getBottom() - normalized * area.getHeight();

    // Draw GR peak value label
    juce::String grText = juce::String (peakGR, 1) + " dB";
    g.setFont (Fonts::getBold (10.0f));
    g.setColour (ColourPalette::grPeakLabel());

    float labelW = 50.0f;
    float labelH = 14.0f;
    float labelX = juce::jlimit (area.getX(), area.getRight() - labelW, x - labelW * 0.5f);
    float labelY = y - labelH - 4.0f;

    if (labelY < area.getY())
        labelY = y + 4.0f;

    // Background pill
    auto labelBounds = juce::Rectangle<float> (labelX, labelY, labelW, labelH);
    g.setColour (ColourPalette::controlBar().withAlpha (0.8f));
    g.fillRoundedRectangle (labelBounds, 3.0f);
    g.setColour (ColourPalette::grPeakLabel());
    g.drawText (grText, labelBounds, juce::Justification::centred, false);

    // Small marker dot at peak
    g.setColour (ColourPalette::grPeakLabel());
    g.fillEllipse (x - 2.0f, y - 2.0f, 4.0f, 4.0f);
}

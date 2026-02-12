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
#include "../LookAndFeel/ColourPalette.h"
#include "HelpTextDatabase.h"

class HelpOverlay : public juce::Component,
                    public juce::Timer
{
public:
    HelpOverlay();
    ~HelpOverlay() override = default;

    void paint (juce::Graphics& g) override;
    void timerCallback() override;
    void parentHierarchyChanged() override;

private:
    static constexpr int kHoverDelayMs = 1300;

    juce::String currentHelpText;
    juce::Rectangle<int> currentTargetBounds;
    juce::String lastHoveredID;
    juce::uint32 hoverStartTime = 0;
    bool tooltipVisible = false;

    void drawHelpTooltip (juce::Graphics& g, const juce::String& text,
                          juce::Rectangle<int> targetBounds);

    juce::Component* findDeepestComponentWithID (juce::Component* root,
                                                  juce::Point<int> pos);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelpOverlay)
};

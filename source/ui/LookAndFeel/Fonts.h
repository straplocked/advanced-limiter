/*
 * Advanced Limiter - Open-source brickwall limiter plugin
 * Copyright (C) 2024-2026 Advanced Limiter contributors
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include <juce_graphics/juce_graphics.h>
#include "BinaryData.h"

class Fonts
{
public:
    static constexpr float kScale = 1.1f;
    static juce::Typeface::Ptr getSpaceGroteskTypeface()
    {
        static auto typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::SpaceGroteskRegular_ttf,
            BinaryData::SpaceGroteskRegular_ttfSize);
        return typeface;
    }

    static juce::Typeface::Ptr getAudiowideTypeface()
    {
        static auto typeface = juce::Typeface::createSystemTypefaceFor (
            BinaryData::AudiowideRegular_ttf,
            BinaryData::AudiowideRegular_ttfSize);
        return typeface;
    }

    static juce::Font getBody (float height)
    {
        return juce::Font (getSpaceGroteskTypeface()).withHeight (height * kScale);
    }

    static juce::Font getBold (float height)
    {
        return juce::Font (getSpaceGroteskTypeface()).withHeight (height * kScale).boldened();
    }

    static juce::Font getTitle (float height)
    {
        return juce::Font (getAudiowideTypeface()).withHeight (height * kScale);
    }
};

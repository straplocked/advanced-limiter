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

#include <juce_audio_basics/juce_audio_basics.h>
#include "LimiterCore.h"
#include "DCFilter.h"
#include "Oversampler.h"
#include "TruePeakLimiter.h"
#include "Dithering.h"
#include "../parameters/LimiterParameterAttachment.h"
#include "../metering/LevelMeterData.h"

class LimiterEngine
{
public:
    LimiterEngine();

    void prepare (double sampleRate, int maxBlockSize, int numChannels);
    void reset();
    void updateParameters (const LimiterParameters& params);
    void process (juce::AudioBuffer<float>& buffer);

    LevelMeterData& getMeterData() { return meterData; }
    int getLatencySamples() const;

private:
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    LimiterCore limiterCore;
    DCFilter dcFilter;
    Oversampler oversampler;
    TruePeakLimiter truePeakLimiter;
    Dithering dithering;

    // Dry buffer for delta mode
    juce::AudioBuffer<float> dryBuffer;

    // Smoothed gains
    float smoothedInputGain = 1.0f;
    float smoothedOutputLevel = 1.0f;

    LimiterParameters cachedParams;
    LevelMeterData meterData;
    int pendingOversampleFactor = -1;

    void applyInputGain (juce::AudioBuffer<float>& buffer);
    void applyOutputCeiling (juce::AudioBuffer<float>& buffer);
    void encodeMidSide (juce::AudioBuffer<float>& buffer);
    void decodeMidSide (juce::AudioBuffer<float>& buffer);
};

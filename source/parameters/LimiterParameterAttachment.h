/*
 * Advanced Limiter - Open-source lookahead limiter plugin
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
#include "ParameterIDs.h"

struct LimiterParameters
{
    float inputGain      = 0.0f;
    float outputLevel    = -0.3f;
    int   limiterStyle   = 4;      // Modern
    float lookaheadMs    = 2.0f;
    float attackMs       = 0.1f;
    float releaseMs      = 100.0f;
    float transientLink  = 100.0f;
    float releaseLink    = 100.0f;
    bool  truePeak       = true;
    bool  dcFilter       = false;
    bool  ditherOn       = false;
    int   ditherBits     = 0;      // 16 bit
    int   noiseShape     = 0;      // Off
    int   oversample     = 0;      // 1x
    bool  scExternal     = false;
    bool  unityGain      = false;
    bool  audition       = false;
    bool  delta          = false;
    int   displayMode    = 1;      // Fast
    int   meterScale     = 1;      // 32 dB
    int   loudnessTarget = 0;      // Off
    int   loudnessTime   = 0;      // Momentary
    bool  channelMode    = false;   // false = L/R, true = M/S
    bool  bypass         = false;
};

class LimiterParameterAttachment
{
public:
    LimiterParameterAttachment() = default;

    void init (juce::AudioProcessorValueTreeState& apvts)
    {
        rawInputGain      = apvts.getRawParameterValue (ParamIDs::kInputGain);
        rawOutputLevel    = apvts.getRawParameterValue (ParamIDs::kOutputLevel);
        rawLimiterStyle   = apvts.getRawParameterValue (ParamIDs::kLimiterStyle);
        rawLookahead      = apvts.getRawParameterValue (ParamIDs::kLookahead);
        rawAttack         = apvts.getRawParameterValue (ParamIDs::kAttack);
        rawRelease        = apvts.getRawParameterValue (ParamIDs::kRelease);
        rawTransientLink  = apvts.getRawParameterValue (ParamIDs::kTransientLink);
        rawReleaseLink    = apvts.getRawParameterValue (ParamIDs::kReleaseLink);
        rawTruePeak       = apvts.getRawParameterValue (ParamIDs::kTruePeak);
        rawDCFilter       = apvts.getRawParameterValue (ParamIDs::kDCFilter);
        rawDitherOn       = apvts.getRawParameterValue (ParamIDs::kDitherOn);
        rawDitherBits     = apvts.getRawParameterValue (ParamIDs::kDitherBits);
        rawNoiseShape     = apvts.getRawParameterValue (ParamIDs::kNoiseShape);
        rawOversample     = apvts.getRawParameterValue (ParamIDs::kOversample);
        rawSCExternal     = apvts.getRawParameterValue (ParamIDs::kSCExternal);
        rawUnityGain      = apvts.getRawParameterValue (ParamIDs::kUnityGain);
        rawAudition       = apvts.getRawParameterValue (ParamIDs::kAudition);
        rawDelta          = apvts.getRawParameterValue (ParamIDs::kDelta);
        rawDisplayMode    = apvts.getRawParameterValue (ParamIDs::kDisplayMode);
        rawMeterScale     = apvts.getRawParameterValue (ParamIDs::kMeterScale);
        rawLoudnessTarget = apvts.getRawParameterValue (ParamIDs::kLoudnessTarget);
        rawLoudnessTime   = apvts.getRawParameterValue (ParamIDs::kLoudnessTime);
        rawChannelMode    = apvts.getRawParameterValue (ParamIDs::kChannelMode);
        rawBypass         = apvts.getRawParameterValue (ParamIDs::kBypass);
    }

    LimiterParameters getCurrentParameters() const
    {
        LimiterParameters p;
        p.inputGain      = rawInputGain->load();
        p.outputLevel    = rawOutputLevel->load();
        p.limiterStyle   = (int) rawLimiterStyle->load();
        p.lookaheadMs    = rawLookahead->load();
        p.attackMs       = rawAttack->load();
        p.releaseMs      = rawRelease->load();
        p.transientLink  = rawTransientLink->load();
        p.releaseLink    = rawReleaseLink->load();
        p.truePeak       = rawTruePeak->load() >= 0.5f;
        p.dcFilter       = rawDCFilter->load() >= 0.5f;
        p.ditherOn       = rawDitherOn->load() >= 0.5f;
        p.ditherBits     = (int) rawDitherBits->load();
        p.noiseShape     = (int) rawNoiseShape->load();
        p.oversample     = (int) rawOversample->load();
        p.scExternal     = rawSCExternal->load() >= 0.5f;
        p.unityGain      = rawUnityGain->load() >= 0.5f;
        p.audition       = rawAudition->load() >= 0.5f;
        p.delta          = rawDelta->load() >= 0.5f;
        p.displayMode    = (int) rawDisplayMode->load();
        p.meterScale     = (int) rawMeterScale->load();
        p.loudnessTarget = (int) rawLoudnessTarget->load();
        p.loudnessTime   = (int) rawLoudnessTime->load();
        p.channelMode    = rawChannelMode->load() >= 0.5f;
        p.bypass         = rawBypass->load() >= 0.5f;
        return p;
    }

private:
    std::atomic<float>* rawInputGain      = nullptr;
    std::atomic<float>* rawOutputLevel    = nullptr;
    std::atomic<float>* rawLimiterStyle   = nullptr;
    std::atomic<float>* rawLookahead      = nullptr;
    std::atomic<float>* rawAttack         = nullptr;
    std::atomic<float>* rawRelease        = nullptr;
    std::atomic<float>* rawTransientLink  = nullptr;
    std::atomic<float>* rawReleaseLink    = nullptr;
    std::atomic<float>* rawTruePeak       = nullptr;
    std::atomic<float>* rawDCFilter       = nullptr;
    std::atomic<float>* rawDitherOn       = nullptr;
    std::atomic<float>* rawDitherBits     = nullptr;
    std::atomic<float>* rawNoiseShape     = nullptr;
    std::atomic<float>* rawOversample     = nullptr;
    std::atomic<float>* rawSCExternal     = nullptr;
    std::atomic<float>* rawUnityGain      = nullptr;
    std::atomic<float>* rawAudition       = nullptr;
    std::atomic<float>* rawDelta          = nullptr;
    std::atomic<float>* rawDisplayMode    = nullptr;
    std::atomic<float>* rawMeterScale     = nullptr;
    std::atomic<float>* rawLoudnessTarget = nullptr;
    std::atomic<float>* rawLoudnessTime   = nullptr;
    std::atomic<float>* rawChannelMode    = nullptr;
    std::atomic<float>* rawBypass         = nullptr;
};

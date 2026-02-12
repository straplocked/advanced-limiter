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

#include "LimiterEngine.h"
#include "../utils/MathUtils.h"
#include <cmath>
#include <algorithm>

LimiterEngine::LimiterEngine()
{
}

void LimiterEngine::prepare (double sampleRate, int maxBlockSize, int numChannels)
{
    currentSampleRate = sampleRate;
    currentBlockSize = maxBlockSize;

    // Prepare sub-components
    oversampler.prepare (sampleRate, maxBlockSize, numChannels);
    // Prepare limiter core at the oversampled rate so timing parameters are correct
    limiterCore.prepare (oversampler.getUpsampledRate(), maxBlockSize * oversampler.getFactor());
    dcFilter.prepare (sampleRate);
    truePeakLimiter.prepare (oversampler.getUpsampledRate());
    dithering.prepare (sampleRate);

    // Allocate dry buffer for delta mode
    dryBuffer.setSize (numChannels, maxBlockSize);

    // Prepare metering
    meterData.prepare (sampleRate);

    // Initialize smoothing
    smoothedInputGain = 1.0f;
    smoothedOutputLevel = 1.0f;

    reset();
}

void LimiterEngine::reset()
{
    limiterCore.reset();
    dcFilter.reset();
    oversampler.reset();
    truePeakLimiter.reset();
    dithering.reset();
    meterData.reset();

    smoothedInputGain = 1.0f;
    smoothedOutputLevel = 1.0f;
}

void LimiterEngine::updateParameters (const LimiterParameters& params)
{
    // Handle oversampling factor changes safely (avoid heap alloc on audio thread)
    if (params.oversample != cachedParams.oversample)
        pendingOversampleFactor = params.oversample;

    cachedParams = params;

    // Update limiter core
    limiterCore.updateParameters (
        params.lookaheadMs,
        params.attackMs,
        params.releaseMs,
        params.transientLink,
        params.releaseLink,
        params.outputLevel,
        static_cast<LimiterStyle> (params.limiterStyle));

    // Update true peak limiter ceiling
    truePeakLimiter.setCeiling (params.outputLevel);

    // Update dithering
    dithering.setEnabled (params.ditherOn);
    dithering.setBitDepth (params.ditherBits);
    dithering.setNoiseShaping (params.noiseShape);
}

void LimiterEngine::process (juce::AudioBuffer<float>& buffer)
{
    juce::ScopedNoDenormals noDenormals;

    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();

    // Handle pending oversampling factor change (safe: between blocks)
    if (pendingOversampleFactor >= 0)
    {
        oversampler.setFactor (pendingOversampleFactor);
        // Re-prepare core at the oversampled rate
        limiterCore.prepare (oversampler.getUpsampledRate(), currentBlockSize * oversampler.getFactor());
        if (cachedParams.truePeak)
            truePeakLimiter.prepare (oversampler.getUpsampledRate());
        pendingOversampleFactor = -1;
    }

    // Bypass: pass audio through unmodified
    if (cachedParams.bypass)
    {
        meterData.pushInputLevel (buffer);
        meterData.pushGainReduction (0.0f, 0.0f);
        meterData.pushOutputLevel (buffer);
        return;
    }

    // Step 1: Apply input gain (smoothed, per-sample)
    applyInputGain (buffer);

    // Step 2: Capture input levels for metering
    meterData.pushInputLevel (buffer);

    // Step 3: Save dry buffer if delta mode is enabled
    if (cachedParams.delta)
    {
        for (int ch = 0; ch < numChannels; ++ch)
            dryBuffer.copyFrom (ch, 0, buffer, ch, 0, numSamples);
    }

    // Step 4: DC filter if enabled
    if (cachedParams.dcFilter)
    {
        float* channelPtrs[2] = { nullptr, nullptr };
        for (int ch = 0; ch < std::min (numChannels, 2); ++ch)
            channelPtrs[ch] = buffer.getWritePointer (ch);

        dcFilter.process (channelPtrs, std::min (numChannels, 2), numSamples);
    }

    // Step 5: Oversampler upsample
    auto oversampledBlock = oversampler.upsample (buffer);

    // Step 6: Get channel pointers from upsampled block
    int oversampledNumSamples = (int) oversampledBlock.getNumSamples();
    int oversampledNumChannels = (int) oversampledBlock.getNumChannels();

    float* channelData[2] = { nullptr, nullptr };
    for (int ch = 0; ch < std::min (oversampledNumChannels, 2); ++ch)
        channelData[ch] = oversampledBlock.getChannelPointer ((size_t) ch);

    // Step 7: LimiterCore.process (get GR values)
    float grL = 0.0f, grR = 0.0f;
    limiterCore.process (channelData, std::min (oversampledNumChannels, 2),
                         oversampledNumSamples, grL, grR);

    // Step 8: True peak limiter if enabled
    if (cachedParams.truePeak)
    {
        for (int ch = 0; ch < std::min (oversampledNumChannels, 2); ++ch)
        {
            for (int i = 0; i < oversampledNumSamples; ++i)
                channelData[ch][i] = truePeakLimiter.processSample (channelData[ch][i], ch);
        }
    }

    // Step 9: Oversampler downsample
    oversampler.downsample (buffer);

    // Step 10: Dithering if enabled
    if (cachedParams.ditherOn)
    {
        for (int ch = 0; ch < std::min (numChannels, 2); ++ch)
        {
            float* data = buffer.getWritePointer (ch);
            for (int i = 0; i < numSamples; ++i)
                data[i] = dithering.processSample (data[i], ch);
        }
    }

    // Step 11: Apply output ceiling (hard clip at ceiling level)
    applyOutputCeiling (buffer);

    // Step 12: Feed true peak meter with output samples
    for (int ch = 0; ch < std::min (numChannels, 2); ++ch)
    {
        const float* data = buffer.getReadPointer (ch);
        for (int i = 0; i < numSamples; ++i)
            meterData.pushTruePeakSample (data[i], ch);
    }

    // Step 13: Capture output levels and GR for metering
    meterData.pushGainReduction (grL, grR);
    meterData.pushOutputLevel (buffer);

    // Step 14: Delta mode: output = dry - wet
    if (cachedParams.delta)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* wet = buffer.getWritePointer (ch);
            const float* dry = dryBuffer.getReadPointer (ch);
            for (int i = 0; i < numSamples; ++i)
                wet[i] = dry[i] - wet[i];
        }
    }

    // Step 15: Unity gain compensation: multiply by inverse of input gain
    if (cachedParams.unityGain && std::abs (cachedParams.inputGain) > 0.01f)
    {
        float compensationGain = MathUtils::dbToGain (-cachedParams.inputGain);
        buffer.applyGain (compensationGain);
    }
}

void LimiterEngine::applyInputGain (juce::AudioBuffer<float>& buffer)
{
    float targetGain = MathUtils::dbToGain (cachedParams.inputGain);

    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();

    // Smooth per-sample to avoid clicks
    constexpr float smoothingCoeff = 0.9995f;

    for (int i = 0; i < numSamples; ++i)
    {
        smoothedInputGain += (1.0f - smoothingCoeff) * (targetGain - smoothedInputGain);

        for (int ch = 0; ch < numChannels; ++ch)
            buffer.getWritePointer (ch)[i] *= smoothedInputGain;
    }
}

void LimiterEngine::applyOutputCeiling (juce::AudioBuffer<float>& buffer)
{
    float ceilingLinear = MathUtils::dbToGain (cachedParams.outputLevel);

    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();

    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* data = buffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
            data[i] = std::clamp (data[i], -ceilingLinear, ceilingLinear);
    }
}

int LimiterEngine::getLatencySamples() const
{
    int latency = limiterCore.getLatencySamples();
    latency += oversampler.getLatencySamples();

    if (cachedParams.truePeak)
        latency += truePeakLimiter.getLatencySamples();

    return latency;
}

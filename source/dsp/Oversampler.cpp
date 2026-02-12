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

#include "Oversampler.h"

void Oversampler::prepare (double sampleRate, int maxBlockSize, int numChannels)
{
    baseSampleRate = sampleRate;
    maxBlock = maxBlockSize;
    channels = numChannels;
    createOversampler (maxBlockSize, numChannels);
}

void Oversampler::reset()
{
    if (oversampler)
        oversampler->reset();
}

void Oversampler::setFactor (int idx)
{
    if (idx == factorIndex)
        return;
    factorIndex = idx;
    factor = 1 << idx;  // 0->1, 1->2, 2->4
    createOversampler (maxBlock, channels);
}

void Oversampler::createOversampler (int maxBlockSize, int numChannels)
{
    factor = 1 << factorIndex;
    upsampledRate = baseSampleRate * factor;

    if (factorIndex == 0)
    {
        oversampler.reset();
        return;
    }

    oversampler = std::make_unique<juce::dsp::Oversampling<float>> (
        numChannels, factorIndex,
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
        true);

    oversampler->initProcessing ((size_t) maxBlockSize);
}

juce::dsp::AudioBlock<float> Oversampler::upsample (juce::AudioBuffer<float>& buffer)
{
    if (!oversampler || factorIndex == 0)
        return juce::dsp::AudioBlock<float> (buffer);

    juce::dsp::AudioBlock<float> inputBlock (buffer);
    return oversampler->processSamplesUp (inputBlock);
}

void Oversampler::downsample (juce::AudioBuffer<float>& buffer)
{
    if (!oversampler || factorIndex == 0)
        return;

    juce::dsp::AudioBlock<float> outputBlock (buffer);
    oversampler->processSamplesDown (outputBlock);
}

int Oversampler::getLatencySamples() const
{
    if (!oversampler || factorIndex == 0)
        return 0;
    return (int) oversampler->getLatencyInSamples();
}

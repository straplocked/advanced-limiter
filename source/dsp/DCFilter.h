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

#include "SVFFilter.h"
#include "FilterTypes.h"
#include <algorithm>

class DCFilter
{
public:
    DCFilter() = default;

    void prepare (double sampleRate)
    {
        for (auto& f : filters)
        {
            f.prepare (sampleRate);
            f.setFirstOrder (true);
            f.setParameters (FilterType::HighPass, 5.0, 0.707, 0.0);
        }
    }

    void reset()
    {
        for (auto& f : filters)
            f.reset();
    }

    void process (float** channelData, int numChannels, int numSamples)
    {
        for (int ch = 0; ch < std::min (numChannels, 2); ++ch)
            filters[ch].process (channelData[ch], numSamples);
    }

private:
    SVFFilter filters[2];
};

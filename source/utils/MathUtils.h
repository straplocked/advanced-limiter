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

#include <cmath>
#include "Constants.h"

namespace MathUtils
{
    inline float dbToGain (float dB)
    {
        return std::pow (10.0f, dB / 20.0f);
    }

    inline float gainToDb (float gain)
    {
        return 20.0f * std::log10 (std::max (gain, 1e-10f));
    }

    inline float gainToY (float gainDB, float height, float minDB, float maxDB)
    {
        return height * (1.0f - (gainDB - minDB) / (maxDB - minDB));
    }

    inline float yToGain (float y, float height, float minDB, float maxDB)
    {
        return minDB + (1.0f - y / height) * (maxDB - minDB);
    }
}

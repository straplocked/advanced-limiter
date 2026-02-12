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

enum class FilterType
{
    Bell = 0,
    LowShelf,
    HighShelf,
    HighPass,
    LowPass,
    Notch,
    BandPass,
    TiltShelf,
    FlatTilt
};

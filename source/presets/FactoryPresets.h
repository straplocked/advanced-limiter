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

#include "PresetData.h"
#include <vector>

namespace FactoryPresets
{
    std::vector<Preset> getMasteringPresets();
    std::vector<Preset> getMixingPresets();
    std::vector<Preset> getBroadcastPresets();
    std::vector<Preset> getCreativePresets();
    std::vector<Preset> getAll();
}

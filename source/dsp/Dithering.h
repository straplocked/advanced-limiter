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

#include <array>
#include <random>

class Dithering
{
public:
    Dithering() = default;

    void prepare (double sampleRate);
    void reset();

    void setEnabled (bool on) { enabled = on; }
    void setBitDepth (int bitsIndex);    // 0=16, 1=18, 2=20, 3=22, 4=24
    void setNoiseShaping (int mode);     // 0=Off, 1=Basic, 2=Optimized, 3=Weighted

    float processSample (float input, int channel);

private:
    bool enabled = false;
    float quantStep = 1.0f / 32768.0f; // 16-bit default
    int shapingMode = 0;
    int targetBits = 16;

    std::mt19937 rng;
    std::uniform_real_distribution<float> dist { -1.0f, 1.0f };

    // Error feedback history per channel (max 9th order)
    static constexpr int kMaxOrder = 9;
    std::array<std::array<float, kMaxOrder>, 2> errorHistory;
    int errorPos[2] = { 0, 0 };

    // Noise shaping coefficients
    static constexpr int kMaxCoeffs = 9;
    std::array<float, kMaxCoeffs> shapeCoeffs;
    int numCoeffs = 0;

    void updateCoefficients();
    float generateTPDF();
};

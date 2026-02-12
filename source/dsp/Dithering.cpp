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

#include "Dithering.h"
#include <cmath>
#include <algorithm>

void Dithering::prepare (double /*sampleRate*/)
{
    rng.seed (42);
    reset();
}

void Dithering::reset()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        errorHistory[(size_t) ch].fill (0.0f);
        errorPos[ch] = 0;
    }
}

void Dithering::setBitDepth (int bitsIndex)
{
    // 0=16, 1=18, 2=20, 3=22, 4=24
    targetBits = 16 + 2 * bitsIndex;
    quantStep = 1.0f / (float) (1 << (targetBits - 1));
}

void Dithering::setNoiseShaping (int mode)
{
    shapingMode = mode;
    updateCoefficients();
}

void Dithering::updateCoefficients()
{
    shapeCoeffs.fill (0.0f);

    switch (shapingMode)
    {
        case 0: // Off
            numCoeffs = 0;
            break;

        case 1: // Basic (1st order error feedback)
            shapeCoeffs[0] = 1.0f;
            numCoeffs = 1;
            break;

        case 2: // Optimized (5th order)
            shapeCoeffs[0] =  1.623f;
            shapeCoeffs[1] = -0.982f;
            shapeCoeffs[2] =  0.109f;
            shapeCoeffs[3] =  0.024f;
            shapeCoeffs[4] = -0.010f;
            numCoeffs = 5;
            break;

        case 3: // Weighted (9th order, psychoacoustically optimized)
            shapeCoeffs[0] =  2.033f;
            shapeCoeffs[1] = -2.165f;
            shapeCoeffs[2] =  1.959f;
            shapeCoeffs[3] = -1.590f;
            shapeCoeffs[4] =  0.6149f;
            shapeCoeffs[5] = -0.2512f;
            shapeCoeffs[6] =  0.4908f;
            shapeCoeffs[7] = -0.3140f;
            shapeCoeffs[8] =  0.1463f;
            numCoeffs = 9;
            break;

        default:
            numCoeffs = 0;
            break;
    }
}

float Dithering::generateTPDF()
{
    // Sum two uniform random values to get triangular distribution
    float r1 = dist (rng);
    float r2 = dist (rng);
    return (r1 + r2) * 0.5f;
}

float Dithering::processSample (float input, int channel)
{
    if (! enabled)
        return input;

    int ch = std::clamp (channel, 0, 1);

    // Generate TPDF noise scaled by quantization step
    float noise = generateTPDF();

    // Compute shaped error from error feedback history
    float shapedError = 0.0f;
    for (int i = 0; i < numCoeffs; ++i)
    {
        // Read from circular error history buffer
        int idx = (errorPos[ch] - 1 - i + kMaxOrder * 2) % kMaxOrder;
        shapedError += errorHistory[(size_t) ch][(size_t) idx] * shapeCoeffs[(size_t) i];
    }

    // Apply dither: add noise, subtract shaped error
    float dithered = input + noise * quantStep - shapedError;

    // Quantize
    float quantized = std::round (dithered / quantStep) * quantStep;

    // Compute and store quantization error
    float error = quantized - dithered;
    errorHistory[(size_t) ch][(size_t) errorPos[ch]] = error;
    errorPos[ch] = (errorPos[ch] + 1) % kMaxOrder;

    return quantized;
}

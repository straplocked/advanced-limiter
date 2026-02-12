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

class ReleaseStage
{
public:
    ReleaseStage() = default;

    void prepare (double sampleRate);
    void reset();

    void setAttackMs (float ms);
    void setReleaseMs (float ms);
    void setHoldMs (float ms);
    void setProgramDependentRelease (bool enabled);

    float processSample (float targetGRdB);

private:
    float currentGR = 0.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    int holdSamples = 0;
    int holdCounter = 0;
    bool programDepRelease = false;
    double sr = 44100.0;

    // Program-dependent release state
    float adaptiveReleaseCoeff = 0.0f;
    float grAccumulator = 0.0f;
    int grDurationSamples = 0;
};

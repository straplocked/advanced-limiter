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

#include <vector>
#include <atomic>

/**
 * @file ScrollingLevelHistory.h
 * @brief Circular ring buffer for storing scrolling level/gain-reduction history
 *        points for the scrolling waveform display.
 */

struct ScrollingHistoryPoint
{
    float inputLevel = -100.0f;    // dB
    float outputLevel = -100.0f;   // dB
    float gainReduction = 0.0f;    // dB (negative)
};

class ScrollingLevelHistory
{
public:
    ScrollingLevelHistory() = default;

    void prepare (int historySize);
    void reset();

    void push (const ScrollingHistoryPoint& point);

    int getSize() const { return size; }
    int getWritePos() const { return writePos; }

    ScrollingHistoryPoint getPoint (int index) const;                  // 0 = oldest
    ScrollingHistoryPoint getPointFromEnd (int offsetFromEnd) const;   // 0 = newest

private:
    std::vector<ScrollingHistoryPoint> buffer;
    int size = 0;
    int writePos = 0;
    int count = 0;
};

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

#include "ScrollingLevelHistory.h"
#include <algorithm>

void ScrollingLevelHistory::prepare (int historySize)
{
    size = historySize;
    buffer.resize ((size_t) size);
    reset();
}

void ScrollingLevelHistory::reset()
{
    std::fill (buffer.begin(), buffer.end(), ScrollingHistoryPoint {});
    writePos = 0;
    count = 0;
}

void ScrollingLevelHistory::push (const ScrollingHistoryPoint& point)
{
    if (size <= 0)
        return;

    buffer[(size_t) writePos] = point;
    writePos = (writePos + 1) % size;

    if (count < size)
        count++;
}

ScrollingHistoryPoint ScrollingLevelHistory::getPoint (int index) const
{
    if (size <= 0 || count <= 0 || index < 0 || index >= count)
        return {};

    // Oldest point is at (writePos - count) wrapped
    int oldest = (writePos - count + size) % size;
    int idx = (oldest + index) % size;
    return buffer[(size_t) idx];
}

ScrollingHistoryPoint ScrollingLevelHistory::getPointFromEnd (int offsetFromEnd) const
{
    if (size <= 0 || count <= 0 || offsetFromEnd < 0 || offsetFromEnd >= count)
        return {};

    // Newest point is at (writePos - 1) wrapped
    int idx = (writePos - 1 - offsetFromEnd + size * 2) % size;
    return buffer[(size_t) idx];
}

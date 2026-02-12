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

#include <juce_audio_processors/juce_audio_processors.h>
#include "PresetData.h"
#include <vector>

class PresetManager
{
public:
    explicit PresetManager (juce::AudioProcessorValueTreeState& apvts);

    const std::vector<Preset>& getAllPresets() const { return allPresets; }
    std::vector<juce::String> getCategories() const;
    std::vector<const Preset*> getPresetsInCategory (const juce::String& category) const;

    void applyPreset (int presetIndex);
    void nextPreset();
    void previousPreset();

    int getCurrentPresetIndex() const { return currentIndex; }
    juce::String getCurrentPresetName() const;

    bool saveCurrentAsPreset (const juce::String& name);
    bool deleteUserPreset (int presetIndex);
    void rescanUserPresets();
    juce::File getUserPresetsDirectory() const;

    // Enhanced browser features
    void toggleFavorite (int presetIndex);
    bool isFavorite (int presetIndex) const;
    std::vector<int> searchPresets (const juce::String& query) const;
    std::vector<int> getFavoriteIndices() const;

    struct Listener
    {
        virtual ~Listener() = default;
        virtual void presetChanged() = 0;
    };

    void addListener (Listener* l)    { listeners.add (l); }
    void removeListener (Listener* l) { listeners.remove (l); }

private:
    juce::AudioProcessorValueTreeState& apvts;
    std::vector<Preset> allPresets;
    int currentIndex = -1;

    juce::ListenerList<Listener> listeners;

    void rebuildPresetList();
    std::vector<Preset> loadUserPresetsFromDisk() const;
    void resetToDefaults();
    void applyPresetData (const LimiterPresetData& data);
    void applyUserPresetFromFile (const juce::File& file);
    void notifyListeners();

    juce::File getFavoritesFile() const;
    void loadFavorites();
    void saveFavorites() const;
    std::vector<juce::String> favoriteNames;
};

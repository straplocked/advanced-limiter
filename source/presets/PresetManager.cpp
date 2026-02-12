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

#include "PresetManager.h"
#include "FactoryPresets.h"
#include "../parameters/ParameterIDs.h"
#include "../utils/Constants.h"

PresetManager::PresetManager (juce::AudioProcessorValueTreeState& vts)
    : apvts (vts)
{
    loadFavorites();
    rebuildPresetList();
}

void PresetManager::rebuildPresetList()
{
    allPresets = FactoryPresets::getAll();
    auto userPresets = loadUserPresetsFromDisk();
    allPresets.insert (allPresets.end(), userPresets.begin(), userPresets.end());

    // Mark favorites
    for (auto& preset : allPresets)
    {
        preset.isFavorite = std::find (favoriteNames.begin(), favoriteNames.end(),
                                        preset.name) != favoriteNames.end();
    }
}

std::vector<Preset> PresetManager::loadUserPresetsFromDisk() const
{
    std::vector<Preset> userPresets;
    auto dir = getUserPresetsDirectory();
    if (! dir.isDirectory()) return userPresets;

    for (auto& file : dir.findChildFiles (juce::File::findFiles, false, "*.xml"))
    {
        Preset p;
        p.name = file.getFileNameWithoutExtension();
        p.category = "User";
        p.isFactory = false;
        p.sourceFile = file;
        userPresets.push_back (std::move (p));
    }

    std::sort (userPresets.begin(), userPresets.end(),
        [] (const Preset& a, const Preset& b) { return a.name.compareIgnoreCase (b.name) < 0; });

    return userPresets;
}

juce::File PresetManager::getUserPresetsDirectory() const
{
    auto appData = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory);
#if JUCE_MAC
    auto dir = appData.getChildFile ("Application Support").getChildFile ("Advanced Limiter").getChildFile ("User Presets");
#else
    auto dir = appData.getChildFile ("Advanced Limiter").getChildFile ("User Presets");
#endif
    return dir;
}

std::vector<juce::String> PresetManager::getCategories() const
{
    std::vector<juce::String> cats;
    for (auto& p : allPresets)
        if (std::find (cats.begin(), cats.end(), p.category) == cats.end())
            cats.push_back (p.category);
    return cats;
}

std::vector<const Preset*> PresetManager::getPresetsInCategory (const juce::String& category) const
{
    std::vector<const Preset*> result;
    for (auto& p : allPresets)
        if (p.category == category)
            result.push_back (&p);
    return result;
}

void PresetManager::applyPreset (int presetIndex)
{
    if (presetIndex < 0 || presetIndex >= (int) allPresets.size()) return;

    auto& preset = allPresets[(size_t) presetIndex];

    if (! preset.isFactory && preset.sourceFile.existsAsFile())
        applyUserPresetFromFile (preset.sourceFile);
    else
    {
        resetToDefaults();
        applyPresetData (preset.data);
    }

    currentIndex = presetIndex;
    notifyListeners();
}

void PresetManager::applyUserPresetFromFile (const juce::File& file)
{
    auto xml = juce::XmlDocument::parse (file);
    if (xml == nullptr) return;
    auto newState = juce::ValueTree::fromXml (*xml);
    if (newState.isValid())
        apvts.replaceState (newState);
}

void PresetManager::resetToDefaults()
{
    auto setParam = [this] (const char* id, float value)
    {
        if (auto* p = apvts.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (value));
    };

    setParam (ParamIDs::kInputGain, Constants::kDefaultInputGain);
    setParam (ParamIDs::kOutputLevel, Constants::kDefaultOutputLevel);
    setParam (ParamIDs::kLookahead, Constants::kDefaultLookahead);
    setParam (ParamIDs::kAttack, Constants::kDefaultAttack);
    setParam (ParamIDs::kRelease, Constants::kDefaultRelease);
    setParam (ParamIDs::kTransientLink, Constants::kDefaultTransientLink);
    setParam (ParamIDs::kReleaseLink, Constants::kDefaultReleaseLink);

    if (auto* p = apvts.getParameter (ParamIDs::kLimiterStyle))
        p->setValueNotifyingHost (p->convertTo0to1 (4.0f)); // Modern

    if (auto* p = apvts.getParameter (ParamIDs::kTruePeak))
        p->setValueNotifyingHost (1.0f); // On
}

void PresetManager::applyPresetData (const LimiterPresetData& data)
{
    auto setParam = [this] (const char* id, float value)
    {
        if (auto* p = apvts.getParameter (id))
            p->setValueNotifyingHost (p->convertTo0to1 (value));
    };

    setParam (ParamIDs::kInputGain, data.inputGain);
    setParam (ParamIDs::kOutputLevel, data.outputLevel);
    setParam (ParamIDs::kLookahead, data.lookaheadMs);
    setParam (ParamIDs::kAttack, data.attackMs);
    setParam (ParamIDs::kRelease, data.releaseMs);
    setParam (ParamIDs::kTransientLink, data.transientLink);
    setParam (ParamIDs::kReleaseLink, data.releaseLink);

    if (auto* p = apvts.getParameter (ParamIDs::kLimiterStyle))
        p->setValueNotifyingHost (p->convertTo0to1 ((float) data.style));
    if (auto* p = apvts.getParameter (ParamIDs::kTruePeak))
        p->setValueNotifyingHost (data.truePeak ? 1.0f : 0.0f);
}

bool PresetManager::saveCurrentAsPreset (const juce::String& name)
{
    if (name.isEmpty()) return false;

    auto dir = getUserPresetsDirectory();
    dir.createDirectory();

    auto safeName = juce::File::createLegalFileName (name);
    auto file = dir.getChildFile (safeName + ".xml");

    auto state = apvts.copyState();
    auto xml = state.createXml();
    if (xml == nullptr) return false;

    if (! xml->writeTo (file)) return false;

    rebuildPresetList();

    for (int i = 0; i < (int) allPresets.size(); ++i)
    {
        if (! allPresets[(size_t) i].isFactory && allPresets[(size_t) i].name == safeName)
        {
            currentIndex = i;
            break;
        }
    }

    notifyListeners();
    return true;
}

bool PresetManager::deleteUserPreset (int presetIndex)
{
    if (presetIndex < 0 || presetIndex >= (int) allPresets.size()) return false;
    auto& preset = allPresets[(size_t) presetIndex];
    if (preset.isFactory) return false;
    if (! preset.sourceFile.deleteFile()) return false;

    if (currentIndex == presetIndex) currentIndex = -1;
    else if (currentIndex > presetIndex) --currentIndex;

    rebuildPresetList();
    notifyListeners();
    return true;
}

void PresetManager::rescanUserPresets()
{
    auto currentName = getCurrentPresetName();
    rebuildPresetList();
    currentIndex = -1;
    for (int i = 0; i < (int) allPresets.size(); ++i)
    {
        if (allPresets[(size_t) i].name == currentName)
        {
            currentIndex = i;
            break;
        }
    }
    notifyListeners();
}

void PresetManager::nextPreset()
{
    if (allPresets.empty()) return;
    int next = currentIndex + 1;
    if (next >= (int) allPresets.size()) next = 0;
    applyPreset (next);
}

void PresetManager::previousPreset()
{
    if (allPresets.empty()) return;
    int prev = currentIndex - 1;
    if (prev < 0) prev = (int) allPresets.size() - 1;
    applyPreset (prev);
}

juce::String PresetManager::getCurrentPresetName() const
{
    if (currentIndex >= 0 && currentIndex < (int) allPresets.size())
        return allPresets[(size_t) currentIndex].name;
    return "Init";
}

void PresetManager::notifyListeners()
{
    listeners.call ([] (Listener& l) { l.presetChanged(); });
}

// --- Enhanced browser features ---

void PresetManager::toggleFavorite (int presetIndex)
{
    if (presetIndex < 0 || presetIndex >= (int) allPresets.size()) return;

    auto& preset = allPresets[(size_t) presetIndex];
    preset.isFavorite = ! preset.isFavorite;

    if (preset.isFavorite)
        favoriteNames.push_back (preset.name);
    else
        favoriteNames.erase (std::remove (favoriteNames.begin(), favoriteNames.end(), preset.name),
                              favoriteNames.end());

    saveFavorites();
}

bool PresetManager::isFavorite (int presetIndex) const
{
    if (presetIndex < 0 || presetIndex >= (int) allPresets.size()) return false;
    return allPresets[(size_t) presetIndex].isFavorite;
}

std::vector<int> PresetManager::searchPresets (const juce::String& query) const
{
    std::vector<int> results;
    auto lowerQuery = query.toLowerCase();

    for (int i = 0; i < (int) allPresets.size(); ++i)
    {
        if (allPresets[(size_t) i].name.toLowerCase().contains (lowerQuery)
            || allPresets[(size_t) i].category.toLowerCase().contains (lowerQuery))
        {
            results.push_back (i);
        }
    }
    return results;
}

std::vector<int> PresetManager::getFavoriteIndices() const
{
    std::vector<int> indices;
    for (int i = 0; i < (int) allPresets.size(); ++i)
        if (allPresets[(size_t) i].isFavorite)
            indices.push_back (i);
    return indices;
}

juce::File PresetManager::getFavoritesFile() const
{
    return getUserPresetsDirectory().getChildFile (".favorites");
}

void PresetManager::loadFavorites()
{
    favoriteNames.clear();
    auto file = getFavoritesFile();
    if (! file.existsAsFile()) return;

    auto lines = file.loadFileAsString();
    juce::StringArray names;
    names.addTokens (lines, "\n", "");
    for (auto& n : names)
    {
        auto trimmed = n.trim();
        if (trimmed.isNotEmpty())
            favoriteNames.push_back (trimmed);
    }
}

void PresetManager::saveFavorites() const
{
    auto file = getFavoritesFile();
    file.getParentDirectory().createDirectory();

    juce::String content;
    for (auto& name : favoriteNames)
        content += name + "\n";

    file.replaceWithText (content);
}

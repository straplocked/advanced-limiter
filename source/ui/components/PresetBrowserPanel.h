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

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../presets/PresetManager.h"
#include "../../presets/PresetMetadata.h"
#include "../LookAndFeel/ColourPalette.h"

class PresetBrowserPanel : public juce::Component,
                           public juce::ListBoxModel,
                           public juce::KeyListener
{
public:
    explicit PresetBrowserPanel (PresetManager& pm)
        : presetManager (pm)
    {
        metadataStore.load();

        // Search box
        searchBox.setTextToShowWhenEmpty ("Search presets...", ColourPalette::textDim());
        searchBox.onTextChange = [this] { rebuildFilteredList(); };
        addAndMakeVisible (searchBox);

        // Category filter
        categoryFilter.addItem ("All Categories", 1);
        auto cats = presetManager.getCategories();
        for (int i = 0; i < (int) cats.size(); ++i)
            categoryFilter.addItem (cats[(size_t) i], i + 2);
        categoryFilter.setSelectedId (1);
        categoryFilter.onChange = [this] { rebuildFilteredList(); };
        addAndMakeVisible (categoryFilter);

        // Favorites toggle
        favToggle.setClickingTogglesState (true);
        favToggle.onClick = [this] { rebuildFilteredList(); };
        addAndMakeVisible (favToggle);

        // Preset list
        listBox.setModel (this);
        listBox.setRowHeight (28);
        listBox.setColour (juce::ListBox::backgroundColourId, ColourPalette::background());
        addAndMakeVisible (listBox);

        // Close button
        closeButton.onClick = [this] { setVisible (false); };
        addAndMakeVisible (closeButton);

        addKeyListener (this);
        rebuildFilteredList();
    }

    ~PresetBrowserPanel() override
    {
        removeKeyListener (this);
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (ColourPalette::background().withAlpha (0.97f));
        g.setColour (ColourPalette::bevelLight());
        g.drawRect (getLocalBounds(), 1);

        g.setColour (ColourPalette::textBright());
        g.setFont (16.5f);
        g.drawText ("Preset Browser", 10, 6, 200, 20, juce::Justification::centredLeft);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (8);
        bounds.removeFromTop (24); // title area

        auto topRow = bounds.removeFromTop (28);
        searchBox.setBounds (topRow.removeFromLeft (topRow.getWidth() / 2 - 4));
        topRow.removeFromLeft (8);
        categoryFilter.setBounds (topRow.removeFromLeft (topRow.getWidth() - 50));
        topRow.removeFromLeft (4);
        favToggle.setBounds (topRow);

        closeButton.setBounds (getWidth() - 30, 6, 22, 22);

        bounds.removeFromTop (6);
        listBox.setBounds (bounds);
    }

    // ListBoxModel
    int getNumRows() override { return (int) filteredEntries.size(); }

    void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (rowNumber < 0 || rowNumber >= (int) filteredEntries.size())
            return;

        auto& entry = filteredEntries[(size_t) rowNumber];

        if (rowIsSelected)
            g.fillAll (ColourPalette::gainReduction().withAlpha (0.25f));

        // Favorite heart
        {
            float hx = 14.0f;
            float hy = (float) height * 0.5f;
            float hs = 5.0f;
            juce::Path heart;
            heart.startNewSubPath (hx, hy + hs * 0.85f);
            heart.cubicTo (hx - hs * 1.2f * 0.52f / 0.48f, hy + hs * 0.1f,
                           hx - hs * 1.0f * 0.52f / 0.48f, hy - hs * 1.0f,
                           hx, hy - hs * 0.3f);
            heart.cubicTo (hx + hs * 1.0f * 0.52f / 0.48f, hy - hs * 1.0f,
                           hx + hs * 1.2f * 0.52f / 0.48f, hy + hs * 0.1f,
                           hx, hy + hs * 0.85f);
            heart.closeSubPath();
            g.setColour (entry.isFavorite ? ColourPalette::gainReduction() : ColourPalette::textDim().withAlpha (0.3f));
            g.fillPath (heart);
        }

        // Name
        g.setColour (ColourPalette::textBright());
        g.setFont (14.3f);
        g.drawText (entry.name, 26, 0, width / 2, height, juce::Justification::centredLeft);

        // Category
        g.setColour (ColourPalette::textDim());
        g.setFont (12.1f);
        g.drawText (entry.category, width / 2, 0, width / 4, height, juce::Justification::centredLeft);

        // Tags
        juce::String tagStr = entry.tags.joinIntoString (", ");
        g.drawText (tagStr, width * 3 / 4, 0, width / 4 - 4, height, juce::Justification::centredRight);
    }

    void listBoxItemClicked (int row, const juce::MouseEvent& e) override
    {
        if (row < 0 || row >= (int) filteredEntries.size())
            return;

        auto& entry = filteredEntries[(size_t) row];

        if (e.mods.isPopupMenu())
        {
            juce::PopupMenu contextMenu;
            contextMenu.addItem (1, entry.isFavorite ? "Unfavorite" : "Favorite");
            if (! presetManager.getAllPresets()[(size_t) entry.presetIndex].isFactory)
                contextMenu.addItem (2, "Delete");

            contextMenu.showMenuAsync ({}, [this, row] (int result) {
                if (row < 0 || row >= (int) filteredEntries.size()) return;
                auto& fe = filteredEntries[(size_t) row];
                if (result == 1) {
                    fe.isFavorite = ! fe.isFavorite;
                    metadataStore.setFavorite (fe.name, fe.isFavorite);
                    listBox.repaint();
                } else if (result == 2) {
                    presetManager.deleteUserPreset (fe.presetIndex);
                    rebuildFilteredList();
                }
            });
            return;
        }

        if (entry.presetIndex >= 0)
            presetManager.applyPreset (entry.presetIndex);
    }

    void listBoxItemDoubleClicked (int row, const juce::MouseEvent&) override
    {
        if (row >= 0 && row < (int) filteredEntries.size())
        {
            auto& entry = filteredEntries[(size_t) row];
            if (entry.presetIndex >= 0)
            {
                presetManager.applyPreset (entry.presetIndex);
                setVisible (false);
            }
        }
    }

    bool keyPressed (const juce::KeyPress& key, juce::Component*) override
    {
        if (key == juce::KeyPress::escapeKey)
        {
            setVisible (false);
            return true;
        }
        if (key == juce::KeyPress::returnKey)
        {
            int sel = listBox.getSelectedRow();
            if (sel >= 0 && sel < (int) filteredEntries.size())
            {
                auto& entry = filteredEntries[(size_t) sel];
                if (entry.presetIndex >= 0)
                    presetManager.applyPreset (entry.presetIndex);
                setVisible (false);
            }
            return true;
        }
        return false;
    }

private:
    void rebuildFilteredList()
    {
        filteredEntries.clear();

        auto& allPresets = presetManager.getAllPresets();
        juce::String query = searchBox.getText();
        bool favOnly = favToggle.getToggleState();
        int catId = categoryFilter.getSelectedId();
        juce::String selectedCat = (catId > 1) ? categoryFilter.getItemText (catId - 1) : "";

        for (int i = 0; i < (int) allPresets.size(); ++i)
        {
            auto& preset = allPresets[(size_t) i];

            PresetMetadataEntry entry;
            entry.name = preset.name;
            entry.category = preset.category;
            entry.presetIndex = i;
            entry.isFavorite = metadataStore.isFavorite (preset.name);

            if (selectedCat.isNotEmpty() && entry.category != selectedCat)
                continue;
            if (favOnly && ! entry.isFavorite)
                continue;
            if (query.isNotEmpty() && ! entry.matchesSearch (query))
                continue;

            filteredEntries.push_back (entry);
        }

        std::sort (filteredEntries.begin(), filteredEntries.end(),
                   [] (const PresetMetadataEntry& a, const PresetMetadataEntry& b)
                   {
                       if (a.isFavorite != b.isFavorite)
                           return a.isFavorite;
                       return a.name.compareIgnoreCase (b.name) < 0;
                   });

        listBox.updateContent();
        listBox.repaint();
    }

    PresetManager& presetManager;
    PresetMetadataStore metadataStore;
    std::vector<PresetMetadataEntry> filteredEntries;

    juce::TextEditor searchBox;
    juce::ComboBox categoryFilter;
    juce::TextButton favToggle { juce::String::charToString (0x2665) };
    juce::TextButton closeButton { "X" };
    juce::ListBox listBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBrowserPanel)
};

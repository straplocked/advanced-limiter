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

#include "HeaderToolbar.h"
#include "../../PluginProcessor.h"
#include "../../parameters/ParameterIDs.h"
#include "../../midi/MIDILearnManager.h"
#include "../LookAndFeel/Fonts.h"

HeaderToolbar::HeaderToolbar (AdvancedLimiterProcessor& p)
    : processor (p),
      presetManager (p.getPresetManager())
{
    presetManager.addListener (this);
    auto& apvts = processor.getAPVTS();

    auto setupIconButton = [this] (juce::TextButton& button, const juce::String& id)
    {
        button.setComponentID (id);
        button.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        button.setColour (juce::TextButton::textColourOffId, ColourPalette::textDim());
        addAndMakeVisible (button);
    };

    // Preset navigation
    setupIconButton (prevButton, "header.prevPreset");
    prevButton.onClick = [this] { presetManager.previousPreset(); };

    setupIconButton (nextButton, "header.nextPreset");
    nextButton.onClick = [this] { presetManager.nextPreset(); };

    presetNameButton.setComponentID ("header.presetName");
    presetNameButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    presetNameButton.setColour (juce::TextButton::textColourOffId, ColourPalette::textBright());
    presetNameButton.onClick = [this] { if (onPresetNameClicked) onPresetNameClicked(); };
    addAndMakeVisible (presetNameButton);

    setupIconButton (saveButton, "header.savePreset");
    saveButton.onClick = [this] { showSaveDialog(); };

    updatePresetName();

    // Undo / Redo
    setupIconButton (undoButton, "header.undo");
    undoButton.onClick = [this] { processor.getUndoManager().undo(); };

    setupIconButton (redoButton, "header.redo");
    redoButton.onClick = [this] { processor.getUndoManager().redo(); };

    // A/B comparison
    abButton.setComponentID ("header.abToggle");
    abButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    abButton.setColour (juce::TextButton::textColourOffId, ColourPalette::textBright());
    abButton.onClick = [this]
    {
        processor.toggleAB();
        abButton.setButtonText (processor.isStateA() ? "A" : "B");
    };
    addAndMakeVisible (abButton);

    copyABButton.setComponentID ("header.copyAB");
    copyABButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    copyABButton.setColour (juce::TextButton::textColourOffId, ColourPalette::textDim());
    copyABButton.onClick = [this] { processor.copyAtoB(); };
    addAndMakeVisible (copyABButton);

    // Delta listen mode
    deltaButton.setComponentID ("header.delta");
    deltaButton.setClickingTogglesState (true);
    deltaButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    deltaButton.setColour (juce::TextButton::textColourOffId, ColourPalette::textDim());
    deltaButton.setColour (juce::TextButton::buttonOnColourId, ColourPalette::gainReduction().withAlpha (0.25f));
    deltaButton.setColour (juce::TextButton::textColourOnId, ColourPalette::textBright());
    addAndMakeVisible (deltaButton);
    deltaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        apvts, ParamIDs::kDelta, deltaButton);

    // MIDI Learn toggle
    midiLearnButton.setComponentID ("header.midiLearn");
    midiLearnButton.setClickingTogglesState (true);
    midiLearnButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    midiLearnButton.setColour (juce::TextButton::textColourOffId, ColourPalette::textDim());
    midiLearnButton.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0x40FF4444));
    midiLearnButton.onClick = [this]
    {
        auto& learnMgr = processor.getMIDILearnManager();
        if (midiLearnButton.getToggleState())
        {
            juce::PopupMenu menu;
            struct ParamEntry { const char* id; const char* name; };
            static const ParamEntry entries[] = {
                { ParamIDs::kInputGain,     "Input Gain" },
                { ParamIDs::kOutputLevel,   "Output Level" },
                { ParamIDs::kLookahead,     "Lookahead" },
                { ParamIDs::kAttack,        "Attack" },
                { ParamIDs::kRelease,       "Release" },
                { ParamIDs::kTransientLink, "Transient Link" },
                { ParamIDs::kReleaseLink,   "Release Link" },
            };
            for (int i = 0; i < 7; ++i)
                menu.addItem (i + 1, entries[i].name);
            menu.addSeparator();
            menu.addItem (100, "Clear All Mappings");

            menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&midiLearnButton),
                [this, &learnMgr] (int result)
                {
                    if (result >= 1 && result <= 7)
                    {
                        static const char* paramIDs[] = {
                            ParamIDs::kInputGain, ParamIDs::kOutputLevel,
                            ParamIDs::kLookahead, ParamIDs::kAttack, ParamIDs::kRelease,
                            ParamIDs::kTransientLink, ParamIDs::kReleaseLink
                        };
                        learnMgr.startLearning (paramIDs[result - 1]);
                    }
                    else if (result == 100)
                    {
                        learnMgr.clearAll();
                    }
                    else
                    {
                        midiLearnButton.setToggleState (false, juce::dontSendNotification);
                    }
                });
        }
        else
        {
            learnMgr.stopLearning();
        }
    };
    addAndMakeVisible (midiLearnButton);
}

HeaderToolbar::~HeaderToolbar()
{
    presetManager.removeListener (this);
    deltaAttachment.reset();
}

void HeaderToolbar::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    juce::ColourGradient grad (ColourPalette::panelTop(), 0, bounds.getY(),
                               ColourPalette::panelBottom(), 0, bounds.getBottom(), false);
    g.setGradientFill (grad);
    g.fillRect (bounds);

    float bottomY = bounds.getBottom();
    g.setColour (ColourPalette::bevelLight());
    g.drawLine (0.0f, bottomY - 1.5f, bounds.getRight(), bottomY - 1.5f, 1.0f);
    g.setColour (ColourPalette::bevelDark());
    g.drawLine (0.0f, bottomY - 0.5f, bounds.getRight(), bottomY - 0.5f, 1.0f);

    // Title with drop shadow
    g.setFont (Fonts::getTitle (17.5f));
    g.setColour (juce::Colour (0x40000000));
    g.drawText ("ADVANCED LIMITER", 14, 1, 275, getHeight(), juce::Justification::centredLeft, true);
    g.setColour (ColourPalette::textBright());
    g.drawText ("ADVANCED LIMITER", 13, 0, 275, getHeight(), juce::Justification::centredLeft, true);
}

void HeaderToolbar::resized()
{
    auto bounds = getLocalBounds().reduced (8, 4);

    int btnH = 24;
    int iconW = 40;

    // === Left side: title gap, then undo/redo, then A/B ===
    bounds.removeFromLeft (286); // title space

    auto undoArea = bounds.removeFromLeft (iconW);
    undoButton.setBounds (undoArea.withHeight (btnH).withCentre (undoArea.getCentre()));

    auto redoArea = bounds.removeFromLeft (iconW);
    redoButton.setBounds (redoArea.withHeight (btnH).withCentre (redoArea.getCentre()));

    bounds.removeFromLeft (7);

    auto abArea = bounds.removeFromLeft (24);
    abButton.setBounds (abArea.withHeight (btnH).withCentre (abArea.getCentre()));
    bounds.removeFromLeft (2);

    auto copyABArea = bounds.removeFromLeft (33);
    copyABButton.setBounds (copyABArea.withHeight (btnH).withCentre (copyABArea.getCentre()));

    bounds.removeFromLeft (9);

    // === Right side: MIDI, Delta ===
    auto midiArea = bounds.removeFromRight (40);
    midiLearnButton.setBounds (midiArea.withHeight (btnH).withCentre (midiArea.getCentre()));
    bounds.removeFromRight (4);

    auto deltaArea = bounds.removeFromRight (44);
    deltaButton.setBounds (deltaArea.withHeight (btnH).withCentre (deltaArea.getCentre()));
    bounds.removeFromRight (4);

    // === Center: preset navigation ===
    auto presetBounds = bounds;
    auto presetArea = presetBounds.withSizeKeepingCentre (
        juce::jmin (286, presetBounds.getWidth()), presetBounds.getHeight());

    auto prevBtnArea = presetArea.removeFromLeft (26);
    prevButton.setBounds (prevBtnArea.withHeight (btnH).withCentre (prevBtnArea.getCentre()));
    presetArea.removeFromLeft (4);

    auto saveBtnArea = presetArea.removeFromRight (42);
    saveButton.setBounds (saveBtnArea.withHeight (btnH).withCentre (saveBtnArea.getCentre()));
    presetArea.removeFromRight (4);

    auto nextBtnArea = presetArea.removeFromRight (26);
    nextButton.setBounds (nextBtnArea.withHeight (btnH).withCentre (nextBtnArea.getCentre()));
    presetArea.removeFromRight (4);

    presetNameButton.setBounds (presetArea.withHeight (btnH).withCentre (presetArea.getCentre()));
}

void HeaderToolbar::presetChanged() { updatePresetName(); }
void HeaderToolbar::updatePresetName() { presetNameButton.setButtonText (presetManager.getCurrentPresetName()); }

void HeaderToolbar::showPresetMenu()
{
    juce::PopupMenu menu;
    auto categories = presetManager.getCategories();
    auto& allPresets = presetManager.getAllPresets();

    auto favIndices = presetManager.getFavoriteIndices();
    if (! favIndices.empty())
    {
        juce::PopupMenu favMenu;
        for (int i : favIndices)
        {
            bool isCurrent = (i == presetManager.getCurrentPresetIndex());
            favMenu.addItem (i + 1, allPresets[(size_t) i].name, true, isCurrent);
        }
        menu.addSubMenu ("Favorites", favMenu);
        menu.addSeparator();
    }

    for (auto& category : categories)
    {
        juce::PopupMenu subMenu;
        for (int i = 0; i < (int) allPresets.size(); ++i)
        {
            if (allPresets[(size_t) i].category == category)
            {
                bool isCurrent = (i == presetManager.getCurrentPresetIndex());
                juce::String label = allPresets[(size_t) i].name;
                if (allPresets[(size_t) i].isFavorite)
                    label = "* " + label;
                subMenu.addItem (i + 1, label, true, isCurrent);
            }
        }
        menu.addSubMenu (category, subMenu);
    }

    menu.addSeparator();
    menu.addItem (-1, "Save Preset...");

    auto idx = presetManager.getCurrentPresetIndex();
    if (idx >= 0 && idx < (int) allPresets.size())
    {
        juce::String favLabel = allPresets[(size_t) idx].isFavorite
            ? "Remove from Favorites" : "Add to Favorites";
        menu.addItem (-3, favLabel);
    }

    if (idx >= 0 && idx < (int) allPresets.size() && ! allPresets[(size_t) idx].isFactory)
        menu.addItem (-2, "Delete \"" + allPresets[(size_t) idx].name + "\"");

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&presetNameButton),
        [this] (int result)
        {
            if (result == -1)
                showSaveDialog();
            else if (result == -2)
                presetManager.deleteUserPreset (presetManager.getCurrentPresetIndex());
            else if (result == -3)
                presetManager.toggleFavorite (presetManager.getCurrentPresetIndex());
            else if (result > 0)
                presetManager.applyPreset (result - 1);
        });
}

void HeaderToolbar::showSaveDialog()
{
    auto* nameEditor = new juce::AlertWindow ("Save Preset", "Enter a name for your preset:", juce::MessageBoxIconType::NoIcon);
    nameEditor->addTextEditor ("name", presetManager.getCurrentPresetName(), "Preset name:");
    nameEditor->addButton ("Save", 1);
    nameEditor->addButton ("Cancel", 0);

    nameEditor->enterModalState (true, juce::ModalCallbackFunction::create (
        [this, nameEditor] (int result)
        {
            if (result == 1)
            {
                auto name = nameEditor->getTextEditorContents ("name");
                presetManager.saveCurrentAsPreset (name);
            }
            delete nameEditor;
        }), false);
}

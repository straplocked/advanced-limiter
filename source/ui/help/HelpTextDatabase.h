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

#include <juce_core/juce_core.h>
#include <unordered_map>

namespace HelpText
{

inline juce::String getHelpText (const juce::String& componentID)
{
    static const std::unordered_map<std::string, const char*> database = {
        // ==================== Header ====================
        { "header.prevPreset",   "Previous Preset \xe2\x80\x94 cycles backward through the preset list." },
        { "header.nextPreset",   "Next Preset \xe2\x80\x94 cycles forward through the preset list." },
        { "header.presetName",   "Current Preset \xe2\x80\x94 click to browse and select factory or user presets." },
        { "header.savePreset",   "Save Preset \xe2\x80\x94 saves the current limiter settings as a user preset." },
        { "header.undo",         "Undo \xe2\x80\x94 reverses the last parameter change (Cmd+Z)." },
        { "header.redo",         "Redo \xe2\x80\x94 re-applies the last undone change (Cmd+Shift+Z)." },
        { "header.abToggle",     "A/B Compare \xe2\x80\x94 toggles between two independent limiter states for quick comparison." },
        { "header.copyAB",       "Copy A to B (or B to A) \xe2\x80\x94 copies the active state to the inactive slot." },
        { "header.delta",        "Delta Listen \xe2\x80\x94 monitors only what the limiter is removing. Useful for hearing clipped peaks and limiting artifacts." },
        { "header.midiLearn",    "MIDI Learn \xe2\x80\x94 assigns MIDI CC messages to limiter parameters for hardware control." },

        // ==================== Bottom Panel ====================
        { "bottom.inputGain",    "Input Gain \xe2\x80\x94 vertical fader that drives the signal into the limiter (-30 to +30 dB). Higher values push more into limiting." },
        { "bottom.outputLevel",  "Output Level / Ceiling \xe2\x80\x94 the maximum output level the limiter will allow (-30 to 0 dB). Set to -0.3 dB for safe mastering or -1.0 dB for streaming." },
        { "bottom.truePeak",     "True Peak \xe2\x80\x94 enables inter-sample peak detection using 4x oversampled metering. Prevents ISP clipping in DACs and lossy codecs." },
        { "bottom.dcFilter",     "DC Filter \xe2\x80\x94 removes sub-sonic DC offset (5 Hz high-pass) that can waste headroom and cause asymmetric clipping." },
        { "bottom.unityGain",    "Unity Gain \xe2\x80\x94 automatically compensates output level to match the input RMS. Useful for A/B comparison without loudness bias." },
        { "bottom.audition",     "Audition \xe2\x80\x94 monitors only the limited/clipped portion of the signal. Hear exactly what is being removed." },
        { "bottom.scExternal",   "Sidechain External \xe2\x80\x94 uses an external sidechain input for the limiter detection circuit instead of the main signal." },
        { "bottom.dither",       "Dither \xe2\x80\x94 adds low-level noise to reduce quantization distortion when reducing bit depth." },
        { "bottom.ditherBits",   "Dither Target Bits \xe2\x80\x94 the bit depth to dither down to (16, 18, 20, 22, or 24 bit)." },
        { "bottom.noiseShape",   "Noise Shaping \xe2\x80\x94 reshapes dither noise to be less audible. Off, Basic (first-order), Optimized (psychoacoustic), or Weighted (ear-weighted)." },
        { "bottom.oversample",   "Oversampling \xe2\x80\x94 processes at higher sample rates for more accurate peak detection and reduced aliasing. Higher = more CPU (1x to 32x)." },
        { "bottom.meterScale",   "Meter Scale \xe2\x80\x94 sets the dB range shown on level meters and scrolling display: 16, 32, 48, K-12, K-14, or K-20." },
        // ==================== Controls (always visible) ====================
        { "controls.lookahead",     "Lookahead \xe2\x80\x94 how far ahead the limiter looks for peaks (0.05 to 5 ms). Longer lookahead = smoother limiting but more latency." },
        { "controls.attack",        "Attack \xe2\x80\x94 how quickly gain reduction engages after a peak is detected (0.01 to 10 ms). Faster = tighter control, slower = more transient punch." },
        { "controls.release",       "Release \xe2\x80\x94 how quickly gain reduction recovers after the signal drops (10 to 1000 ms). Faster = more punch, slower = more transparent." },
        { "controls.transientLink", "Transient Link \xe2\x80\x94 how much L/R channels share transient gain reduction. 100% = fully linked stereo, 0% = independent dual-mono." },
        { "controls.releaseLink",   "Release Link \xe2\x80\x94 how much L/R channels share release behavior. 100% = linked, 0% = independent. Affects stereo image during recovery." },
        { "controls.style",         "Limiter Style \xe2\x80\x94 selects the character and behavior: Transparent, Punchy, Dynamic, Allround, Modern, Aggressive, Bus, or Safe." },
        { "controls.displayMode",   "Display Mode \xe2\x80\x94 controls the scrolling speed of the level display: SlowDown, Fast, Slow, Infinite (no decay), or Off." },

        // ==================== Loudness ====================
        { "loudness.reset",      "Reset Loudness \xe2\x80\x94 resets the integrated loudness and LRA measurements to start fresh." },
    };

    auto it = database.find (componentID.toStdString());
    if (it != database.end())
        return juce::String::fromUTF8 (it->second);

    return {};
}

} // namespace HelpText

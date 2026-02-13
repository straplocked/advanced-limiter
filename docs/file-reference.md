# File Reference

## Source Tree

### Root
| File | Description |
|------|-------------|
| `PluginProcessor.h/cpp` | JUCE AudioProcessor. Entry point for audio processing, state save/restore, parameter layout creation. |
| `PluginEditor.h/cpp` | JUCE AudioProcessorEditor. Creates and lays out all UI components. Listens for display parameter changes. |

### `dsp/` -- DSP Engine
| File | Description |
|------|-------------|
| `LimiterEngine.h/cpp` | Top-level DSP processor. Orchestrates all DSP modules, manages oversampling, metering, and parameter updates. |
| `LimiterCore.h/cpp` | Per-channel limiter core. Lookahead delay, peak detection, gain reduction with transient and release stages. |
| `LimiterStyle.h` | Style enum and `LimiterStyleBehavior` table defining the 8 limiter characters. |
| `TransientStage.h/cpp` | Fast one-pole attack envelope for transient detection. Per-channel with cross-channel linking. |
| `ReleaseStage.h/cpp` | Release envelope with optional program-dependent adaptive behavior. |
| `TruePeakLimiter.h/cpp` | Post-oversampler true peak safety limiter. |
| `TruePeakDetector.h` | 4x polyphase oversampled inter-sample peak detector. |
| `Oversampler.h/cpp` | Wrapper around JUCE `dsp::Oversampling` (1x to 32x). |
| `Dithering.h/cpp` | TPDF dither with 4 noise shaping modes and configurable target bit depth. |
| `DCFilter.h` | Simple 5 Hz first-order high-pass filter. |
| `SVFFilter.h/cpp` | Cytomic TPT state-variable filter. |
| `FilterTypes.h` | Filter type enums. |

### `metering/` -- Metering
| File | Description |
|------|-------------|
| `LevelMeterData.h/cpp` | Meter frame struct and TripleBuffer-based data transfer from audio to UI thread. |
| `LoudnessMeter.h` | EBU R128 loudness meter (momentary, short-term, integrated LUFS). |
| `LoudnessRangeMeter.h` | LRA (Loudness Range) calculation. |
| `TruePeakMeter.h/cpp` | True peak measurement for metering display. |
| `PeakHoldCalculator.h` | Peak hold with configurable hold time and decay rate. |
| `ScrollingLevelHistory.h/cpp` | Circular buffer for scrolling display history points. |

### `analyzer/`
| File | Description |
|------|-------------|
| `TripleBuffer.h` | Lock-free triple buffer for wait-free single-producer single-consumer data transfer. |

### `parameters/` -- Parameter System
| File | Description |
|------|-------------|
| `ParameterIDs.h` | All parameter ID constants and choice name arrays. |
| `ParameterLayout.h/cpp` | APVTS parameter layout creation with ranges and defaults. |
| `LimiterParameterAttachment.h` | Atomic parameter reader for lock-free audio thread access. |

### `presets/` -- Preset System
| File | Description |
|------|-------------|
| `PresetManager.h/cpp` | Preset loading, saving, browsing, favorites, and user preset management. |
| `FactoryPresets.h/cpp` | 16 factory presets across 4 categories. |
| `PresetData.h` | Preset data struct with limiter parameter values. |
| `PresetMetadata.h` | Per-user metadata store for favorites. |

### `midi/` -- MIDI
| File | Description |
|------|-------------|
| `MIDILearnManager.h` | MIDI CC learn/assign system for continuous parameters. |

### `ui/components/` -- UI Components
| File | Description |
|------|-------------|
| `HeaderToolbar.h/cpp` | Top toolbar: title, presets, undo/redo, A/B, delta, MIDI learn. |
| `InputGainStrip.h/cpp` | Vertical input gain fader strip on the left side. |
| `LimiterDisplay.h/cpp` | Scrolling waveform display with input/output/GR visualization. |
| `LevelMeterComponent.h/cpp` | 5-bar vertical level meter with peak hold and true peak markers. |
| `LoudnessPanel.h/cpp` | Loudness readout overlay (M/S/I LUFS, LRA, target indicator). |
| `BottomControlStrip.h/cpp` | Two-row control panel with knobs, toggle buttons, and combo boxes. |
| `PresetBrowserPanel.h` | Preset browser overlay with search, filter, and favorites. |

### `ui/help/` -- Help System
| File | Description |
|------|-------------|
| `HelpOverlay.h/cpp` | Mouse-hover tooltip overlay with delayed appearance. |
| `HelpTextDatabase.h` | Static database of component ID to help text mappings. |

### `ui/LookAndFeel/` -- Visual Style
| File | Description |
|------|-------------|
| `AdvancedLimiterLookAndFeel.h` | Custom L&F with rotary slider, linear slider, button, combo box, and icon rendering. |
| `ColourPalette.h` | Centralized colour definitions for the entire UI. |
| `Fonts.h` | Font loading (Space Grotesk, Audiowide) with 1.1x global scale factor. |

### `utils/` -- Utilities
| File | Description |
|------|-------------|
| `Constants.h` | Plugin-wide constants: dimensions, ranges, defaults, meter settings. |
| `MathUtils.h` | Math helper functions. |

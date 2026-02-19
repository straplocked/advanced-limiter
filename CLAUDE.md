# CLAUDE.md — Advanced Limiter

## Project Overview

Advanced Limiter is a professional brickwall lookahead limiter audio plugin built with **JUCE 7.0.12** and **C++20**. It features 8 limiting styles, true peak detection, EBU R128 loudness metering, and up to 32x oversampling. Licensed GPL-3.0-or-later.

## Build Commands

```bash
# Configure + build (all platforms)
cmake -B build
cmake --build build

# Release build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Linux: install system deps first
sudo apt-get install libasound2-dev libfreetype6-dev libx11-dev libxrandr-dev libxcursor-dev libxinerama-dev libgl1-mesa-dev libxcomposite-dev

# Windows (Visual Studio)
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release

# With tests (no test suite exists yet)
cmake -B build -DADVANCED_LIMITER_BUILD_TESTS=ON
```

Build output goes to `build/AdvancedLimiter_artefacts/{Standalone,VST3,AU}/`. On macOS, plugins auto-install to `~/Library/Audio/Plug-Ins/`.

## Plugin Formats

Standalone, VST3 (all platforms), AU (macOS), CLAP (optional submodule), AAX (optional `-DAAX_SDK_PATH`).

## Architecture

**Entry points:**
- `source/PluginProcessor.h/cpp` — JUCE AudioProcessor, owns LimiterEngine + APVTS (23 params)
- `source/PluginEditor.h/cpp` — JUCE AudioProcessorEditor, owns all UI components

**Threading model (critical):**
- **Audio thread**: Lock-free only. No allocations, mutexes, or I/O. Reads params via `std::atomic<float>*`. Publishes meter data via `TripleBuffer`.
- **UI thread**: 30 Hz timer. Reads meter frames via `TripleBuffer::swapAndCache()`. Updates and repaints components.
- **Parameter bridge**: `LimiterParameterAttachment` provides atomic parameter reads for the audio thread.

**DSP signal flow:**
```
Input → Gain → DC Filter → Oversample↑ → LimiterCore (per-channel) → Oversample↓ → TruePeakLimiter → Dither → Output Level → Metering
```

**Key DSP classes** (`source/dsp/`):
- `LimiterEngine` — top-level orchestrator
- `LimiterCore` — per-channel limiter (TransientStage + ReleaseStage)
- `LimiterStyle` — 8 table-driven style behaviors
- `TruePeakLimiter/TruePeakDetector` — inter-sample peak safety
- `Oversampler` — 1x–32x polyphase wrapper

**UI layout** (`source/ui/`):
- `HeaderToolbar` (40px) — title, undo/redo, A/B, presets, delta
- `InputGainStrip` (50px) — vertical gain fader
- `LimiterDisplay` — scrolling waveform (center)
- `LevelMeterComponent` (110px) — 5-bar vertical meter
- `BottomControlStrip` (154px) — knobs and buttons in 2 rows

## Source Layout

```
source/
├── PluginProcessor.h/cpp       # AudioProcessor entry point
├── PluginEditor.h/cpp          # GUI entry point
├── dsp/                        # All DSP processing
├── metering/                   # Level/loudness meters + TripleBuffer
├── analyzer/                   # TripleBuffer (lock-free)
├── parameters/                 # ParameterIDs, ParameterLayout, atomic attachment
├── presets/                    # PresetManager, 16 factory presets
├── midi/                       # MIDI learn
├── ui/components/              # UI components
├── ui/help/                    # HelpOverlay + tooltip database
├── ui/LookAndFeel/             # Custom L&F, colors, fonts
└── utils/                      # Constants, math helpers
```

## Code Style

- **Indentation**: 4 spaces, no tabs
- **Braces**: Allman style — opening brace on new line for functions/classes
- **Spacing**: Spaces inside parentheses: `if ( condition )`, `foo( bar, baz )`
- **File naming**: PascalCase matching class name (`LimiterCore.h`, `LimiterCore.cpp`)
- **Members**: `camelCase`. Constants use `constexpr` with `k` prefix (`kMeterMinDB`)
- **Includes**: JUCE via `<juce_audio_processors/juce_audio_processors.h>`, project via `"dsp/LimiterCore.h"`
- **Header-only**: Simple processors (DCFilter.h, TruePeakDetector.h) are header-only
- **Copyright**: All files start with GPL-3.0 header (see CONTRIBUTING.md)
- **Warnings**: `-Wall -Wextra -Wpedantic -Wno-unused-parameter` (GCC/Clang), `/W4 /permissive-` (MSVC)
- **Fast math**: `-ffast-math` / `/fp:fast` enabled for DSP performance

## DSP Class Pattern

All DSP classes follow:
```cpp
void prepare( double sampleRate, int samplesPerBlock );
void reset();
void process( juce::AudioBuffer<float>& buffer );
```

## How to Add Things

**New parameter**: ParameterIDs.h → Constants.h → ParameterLayout.cpp → LimiterParameterAttachment.h → use in LimiterEngine

**New limiter style**: LimiterStyle.h (enum + behavior) → ParameterIDs.h (name) → Constants.h (kNumStyles)

**New factory preset**: FactoryPresets.cpp

**New UI component**: Create in `source/ui/components/`, add to PluginEditor layout

## Key Files for Common Tasks

| Task | Files |
|------|-------|
| DSP behavior | `source/dsp/LimiterEngine.cpp`, `LimiterCore.cpp` |
| Parameter changes | `source/parameters/ParameterIDs.h`, `ParameterLayout.cpp` |
| UI layout | `source/PluginEditor.cpp`, `source/ui/components/` |
| Colors/theming | `source/ui/LookAndFeel/ColourPalette.h` |
| Metering | `source/metering/LevelMeterData.h`, `LoudnessMeter.h` |
| Build config | `CMakeLists.txt`, `cmake/PlatformDefaults.cmake` |
| Presets | `source/presets/FactoryPresets.cpp`, `PresetManager.cpp` |

## CI/CD

GitHub Actions (`.github/workflows/build.yml`): builds on macOS-14, Windows-latest, Ubuntu-latest on push/PR to master and manual dispatch. No automated tests yet.

## Dependencies

- **JUCE 7.0.12** — git submodule at `JUCE/`
- **clap-juce-extensions** — optional submodule at `libs/clap-juce-extensions/`
- CMake 3.25+, C++20 compiler (GCC 12+, Clang 14+, MSVC 2022+)

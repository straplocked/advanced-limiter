# Architecture

## Overview

Advanced Limiter follows JUCE best practices: a JUCE `AudioProcessor` with an `AudioProcessorValueTreeState` (APVTS) for parameter management, a dedicated DSP engine, and a component-based GUI. The design prioritizes lock-free audio processing and clean separation between the audio thread and the UI thread.

## System Diagram

```
┌─────────────────────────────────────────────────────┐
│  AdvancedLimiterProcessor (AudioProcessor)          │
│                                                     │
│  ┌─────────────────────┐  ┌──────────────────────┐  │
│  │  APVTS              │  │  LimiterParam-       │  │
│  │  (23 parameters)    │──│  Attachment          │  │
│  │                     │  │  (atomic<float>*)    │  │
│  └─────────────────────┘  └──────────────────────┘  │
│                                      │              │
│  ┌─────────────────────┐             │              │
│  │  PresetManager      │             │              │
│  │  (load/save/browse) │             ▼              │
│  └─────────────────────┘  ┌──────────────────────┐  │
│                           │  LimiterEngine       │  │
│                           │  (top-level DSP)     │  │
│                           └──────────────────────┘  │
│                                      │              │
│                                      ▼              │
│                           ┌──────────────────────┐  │
│                           │  LevelMeterData      │  │
│                           │  (TripleBuffer)      │  │
│                           └──────────────────────┘  │
└─────────────────────────────────────────────────────┘
                                       │
                                       ▼
┌─────────────────────────────────────────────────────┐
│  AdvancedLimiterEditor (AudioProcessorEditor)       │
│                                                     │
│  ┌─────────────────────────────────────────────┐    │
│  │  HeaderToolbar (40 px)                      │    │
│  │  Title | Undo/Redo | A/B | Presets | Delta  │    │
│  ├──────┬──────────────────────────────┬───────┤    │
│  │Input │  LimiterDisplay              │Level  │    │
│  │Gain  │  (scrolling waveform)        │Meter  │    │
│  │Strip │                              │Comp.  │    │
│  │(50px)│  [LoudnessPanel overlay]     │(110px)│    │
│  ├──────┴──────────────────────────────┴───────┤    │
│  │  BottomControlStrip (154 px)                │    │
│  │  Row 1: Style|LA|Atk|Rel|TLink|RLink|Out   │    │
│  │  Row 2: [TP DC UG Aud SC] | [Dither/combos]│    │
│  └─────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────┘
```

## Threading Model

### Audio Thread

The audio thread runs `processBlock()` which:

1. Reads all parameters atomically via `LimiterParameterAttachment::getCurrentParameters()`
2. Passes them to `LimiterEngine::updateParameters()`
3. Runs `LimiterEngine::process()` on the audio buffer
4. Meter data is published via `TripleBuffer` (wait-free single-producer)

**Rules**: No allocations, no locks, no I/O, no virtual calls on unowned objects.

### UI Thread

The UI thread runs on a 30 Hz timer (`Constants::kUITimerHz`):

1. `LimiterDisplay` reads meter frames from `TripleBuffer` (single-consumer via `swapAndCache()`)
2. Other components (`LevelMeterComponent`, `LoudnessPanel`) read from the cached frame
3. Updates visual components
4. Triggers `repaint()`

### Data Transfer

| Data | Mechanism | Direction |
|------|-----------|-----------|
| Parameters | `std::atomic<float>*` | UI -> Audio |
| Meter levels | `TripleBuffer<MeterFrame>` | Audio -> UI |
| Presets | APVTS `setParameterNotifyingHost()` | UI -> Audio |
| State | XML serialization (not real-time) | Host <-> Plugin |

## Module Dependency Graph

```
PluginProcessor
  ├── LimiterEngine
  │     ├── LimiterCore (per-channel)
  │     │     ├── TransientStage
  │     │     └── ReleaseStage
  │     ├── TruePeakLimiter
  │     ├── TruePeakDetector
  │     ├── Oversampler
  │     ├── Dithering
  │     ├── DCFilter
  │     ├── LoudnessMeter
  │     ├── LoudnessRangeMeter
  │     └── LevelMeterData
  ├── LimiterParameterAttachment
  ├── PresetManager
  └── MIDILearnManager

PluginEditor
  ├── HeaderToolbar
  ├── InputGainStrip
  ├── LimiterDisplay
  │     └── ScrollingLevelHistory
  ├── LevelMeterComponent
  │     └── PeakHoldCalculator
  ├── LoudnessPanel
  ├── BottomControlStrip
  ├── PresetBrowserPanel
  ├── HelpOverlay
  │     └── HelpTextDatabase
  └── AdvancedLimiterLookAndFeel
```

## State Serialization

Plugin state is saved/restored as XML via APVTS:

```xml
<AdvancedLimiterState>
  <PARAM id="input_gain" value="0.0"/>
  <PARAM id="output_level" value="-0.3"/>
  <!-- ... all 23 parameters ... -->
</AdvancedLimiterState>
```

The `PresetManager` also saves/loads user presets as XML files to the platform-appropriate Application Support directory.

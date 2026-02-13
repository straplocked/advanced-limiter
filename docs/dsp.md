# DSP Architecture

## Signal Flow

```
Input Buffer
  │
  ├── Input Gain
  │
  ├── DC Filter (optional, 5 Hz HPF)
  │
  ├── Oversampler (upsample 1x-32x)
  │     │
  │     ├── LimiterCore (per channel)
  │     │     ├── Lookahead delay line
  │     │     ├── Peak detection
  │     │     ├── TransientStage (fast attack envelope)
  │     │     ├── ReleaseStage (release envelope, optional program-dependent)
  │     │     └── Gain application
  │     │
  │     ├── Channel linking (Transient Link + Release Link)
  │     │
  │     └── Style behavior modifiers
  │
  ├── Oversampler (downsample)
  │
  ├── True Peak Limiter (post-oversampler safety net)
  │
  ├── Dithering (optional, with noise shaping)
  │
  ├── Output Level (ceiling)
  │
  └── Metering
        ├── LevelMeterData (input/output/GR per channel)
        ├── TruePeakMeter (4x oversampled ISP detection)
        ├── LoudnessMeter (EBU R128 momentary/short-term/integrated)
        └── LoudnessRangeMeter (LRA calculation)
```

## Core Algorithms

### Lookahead Limiting

The limiter uses a delay-line based lookahead scheme. The audio is delayed by the lookahead time, while the gain reduction envelope is computed from the non-delayed signal. This allows the limiter to "see" peaks before they arrive and apply smooth gain reduction.

### Gain Reduction

Gain reduction is computed per-sample:

1. **Peak detection**: Absolute sample value is compared against the output ceiling
2. **Required GR**: `max(0, peakDB - ceilingDB)` in dB
3. **TransientStage**: Fast one-pole attack envelope with style-dependent time constant
4. **ReleaseStage**: Release envelope with optional program-dependent adaptive release
5. **Channel linking**: Transient and release GR are blended between L/R based on link percentages

### Style System

Each of the 8 limiter styles is defined by a `LimiterStyleBehavior` struct:

| Field | Description |
|-------|-------------|
| `lookaheadScale` | Multiplier for user lookahead setting |
| `attackScale` | Multiplier for user attack setting |
| `releaseScale` | Multiplier for user release setting |
| `transientPreserve` | 0-1, how much transient detail to preserve |
| `programDepRelease` | Enable program-dependent adaptive release |
| `holdMs` | Hold time before release begins |
| `clipAmount` | 0-1, amount of hard clipping to apply |

### True Peak Detection

The `TruePeakDetector` uses 4x polyphase upsampling to detect inter-sample peaks that exceed 0 dBFS. This is critical for broadcast-compliant limiting (EBU R128, ITU-R BS.1770).

The `TruePeakLimiter` is a post-oversampler safety net that catches any remaining peaks after the main limiter.

### Dithering

The `Dithering` class supports:

- **TPDF dither**: Triangular probability density function noise
- **Noise shaping**: 4 modes (Off, Basic, Optimized, Weighted) that reshape dither noise to be less audible by pushing it into less-sensitive frequency ranges

### Oversampling

The `Oversampler` wraps JUCE's `dsp::Oversampling` with up to 32x oversampling using half-band polyphase IIR filters. Higher oversampling reduces aliasing from the nonlinear limiting process.

## Metering

### Level Metering

`LevelMeterData` captures per-frame:
- Input L/R peak levels
- Output L/R peak levels
- Gain reduction L/R
- True peak L/R (from 4x oversampled detector)

Data is transferred to the UI via a lock-free `TripleBuffer`.

### Loudness Metering

`LoudnessMeter` implements EBU R128 / ITU-R BS.1770:
- **Momentary**: 400 ms sliding window
- **Short-term**: 3 s sliding window
- **Integrated**: Gated measurement over entire duration

`LoudnessRangeMeter` computes the LRA (Loudness Range) statistic.

## Filters

### DCFilter

A simple 5 Hz first-order high-pass filter to remove DC offset that wastes headroom.

### SVFFilter

Cytomic TPT (Topology-Preserving Transform) state-variable filter used internally for various filtering needs.

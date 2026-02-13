# Preset System

## Factory Presets

Advanced Limiter ships with 16 factory presets across 4 categories:

### Mastering
| Preset | Style | Output | Lookahead | Description |
|--------|-------|--------|-----------|-------------|
| Mastering Transparent | Transparent | -0.3 dB | 2.0 ms | Clean, neutral mastering limiter |
| Mastering Loud | Modern | -0.1 dB | 1.0 ms | Competitive loudness with modern character |
| Mastering Safe | Safe | -0.5 dB | 3.0 ms | Maximum transparency, conservative limiting |
| Mastering Punchy | Punchy | -0.3 dB | 1.5 ms | Transient-preserving mastering |

### Mixing
| Preset | Style | Description |
|--------|-------|-------------|
| Mix Bus Glue | Bus | Gentle bus limiting for mix cohesion |
| Vocal Limiter | Dynamic | Program-dependent limiting for vocals |
| Drum Limiter | Punchy | Transient-preserving drum limiting |
| Bass Limiter | Allround | Balanced limiting for bass instruments |

### Broadcast
| Preset | Style | Target | Description |
|--------|-------|--------|-------------|
| EBU R128 | Transparent | -23 LUFS | European broadcast standard |
| Podcast | Allround | -16 LUFS | Podcast loudness target |
| Streaming -14 LUFS | Modern | -14 LUFS | Spotify/YouTube loudness |
| Film -23 LUFS | Safe | -23 LUFS | Film/cinema standard |

### Creative
| Preset | Style | Description |
|--------|-------|-------------|
| Brick Wall | Aggressive | Maximum loudness, aggressive clipping |
| Pumping | Dynamic | Exaggerated pumping effect |
| Saturator | Modern | Moderate clipping for saturation character |
| Soft Clip | Allround | Gentle clipping for warmth |

## Preset File Format

Presets are stored as XML files with the same format as JUCE APVTS state:

```xml
<AdvancedLimiterState>
  <PARAM id="input_gain" value="0.0"/>
  <PARAM id="output_level" value="-0.3"/>
  <PARAM id="limiter_style" value="0.0"/>
  <!-- ... -->
</AdvancedLimiterState>
```

## User Presets

User presets are saved to the platform-appropriate Application Support directory:

| Platform | Path |
|----------|------|
| macOS | `~/Library/Application Support/AdvancedLimiter/Presets/` |
| Windows | `%APPDATA%/AdvancedLimiter/Presets/` |
| Linux | `~/.config/AdvancedLimiter/Presets/` |

## Preset Metadata

The `PresetMetadataStore` tracks per-user metadata (favorites) separately from preset files, stored as a JSON file alongside user presets. This allows favorites to persist across factory preset updates.

## Adding Factory Presets

Factory presets are defined in `source/presets/FactoryPresets.cpp`. Each category has a getter function that returns a `std::vector<Preset>`:

```cpp
presets.push_back ({ "Preset Name", "Category",
    { inputGain, outputLevel, styleIndex, lookaheadMs, attackMs, releaseMs,
      transientLink, releaseLink, truePeak } });
```

All factory presets have `isFactory = true` and cannot be deleted by the user.

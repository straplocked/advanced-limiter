# Parameters

Advanced Limiter has 23 automatable parameters managed through JUCE's `AudioProcessorValueTreeState`.

## Parameter Table

| ID | Name | Type | Range | Default | Unit |
|----|------|------|-------|---------|------|
| `input_gain` | Input Gain | Float | -30 to +30 | 0.0 | dB |
| `output_level` | Output Level | Float | -30 to 0 | -0.3 | dB |
| `limiter_style` | Limiter Style | Choice | 0-7 | 0 | -- |
| `lookahead` | Lookahead | Float | 0.05 to 5.0 | 2.0 | ms |
| `attack` | Attack | Float | 0.01 to 10.0 | 0.1 | ms |
| `release` | Release | Float | 10 to 1000 | 100 | ms |
| `transient_link` | Transient Link | Float | 0 to 100 | 100 | % |
| `release_link` | Release Link | Float | 0 to 100 | 100 | % |
| `true_peak` | True Peak | Bool | off/on | on | -- |
| `dc_filter` | DC Filter | Bool | off/on | off | -- |
| `dither_on` | Dither | Bool | off/on | off | -- |
| `dither_bits` | Dither Bits | Choice | 0-4 | 0 | -- |
| `noise_shape` | Noise Shape | Choice | 0-3 | 0 | -- |
| `oversample` | Oversample | Choice | 0-5 | 0 | -- |
| `sc_external` | SC External | Bool | off/on | off | -- |
| `unity_gain` | Unity Gain | Bool | off/on | off | -- |
| `audition` | Audition | Bool | off/on | off | -- |
| `delta` | Delta | Bool | off/on | off | -- |
| `display_mode` | Display Mode | Choice | 0-4 | 1 | -- |
| `meter_scale` | Meter Scale | Choice | 0-5 | 1 | -- |
| `loudness_target` | Loudness Target | Choice | 0-3 | 0 | -- |
| `loudness_time` | Loudness Time | Choice | 0-2 | 0 | -- |
| `bypass` | Bypass | Bool | off/on | off | -- |

## Choice Parameter Values

### Limiter Style (`limiter_style`)

| Value | Name | Description |
|-------|------|-------------|
| 0 | Transparent | Clean, neutral limiting |
| 1 | Punchy | Fast attack, transient-preserving |
| 2 | Dynamic | Program-dependent release |
| 3 | Allround | Balanced general-purpose |
| 4 | Modern | Short lookahead, moderate clipping |
| 5 | Aggressive | Very fast, hard clipping |
| 6 | Bus | Long lookahead, gentle limiting |
| 7 | Safe | Maximum lookahead, ultra-transparent |

### Dither Bits (`dither_bits`)

| Value | Name |
|-------|------|
| 0 | 16 bit |
| 1 | 18 bit |
| 2 | 20 bit |
| 3 | 22 bit |
| 4 | 24 bit |

### Noise Shape (`noise_shape`)

| Value | Name | Description |
|-------|------|-------------|
| 0 | Off | No noise shaping |
| 1 | Basic | First-order noise shaping |
| 2 | Optimized | Psychoacoustically optimized |
| 3 | Weighted | Ear-weighted noise shaping |

### Oversample (`oversample`)

| Value | Name |
|-------|------|
| 0 | 1x |
| 1 | 2x |
| 2 | 4x |
| 3 | 8x |
| 4 | 16x |
| 5 | 32x |

### Display Mode (`display_mode`)

| Value | Name | Description |
|-------|------|-------------|
| 0 | SlowDown | 4x decimation, extended time view |
| 1 | Fast | 1:1, real-time scrolling |
| 2 | Slow | 2x decimation |
| 3 | Infinite | No decay, peaks hold |
| 4 | Off | Display disabled |

### Meter Scale (`meter_scale`)

| Value | Name |
|-------|------|
| 0 | 16 dB |
| 1 | 32 dB |
| 2 | 48 dB |
| 3 | K-12 |
| 4 | K-14 |
| 5 | K-20 |

### Loudness Target (`loudness_target`)

| Value | Name |
|-------|------|
| 0 | Off |
| 1 | -23 LUFS (EBU R128) |
| 2 | -16 LUFS (Streaming) |
| 3 | -14 LUFS (Streaming) |

## Parameter Architecture

Parameters flow through the system as follows:

1. **APVTS** stores authoritative parameter values and handles host automation
2. **LimiterParameterAttachment** loads all parameter values into a `LimiterParameters` struct using `std::atomic<float>*` pointers (lock-free)
3. **LimiterEngine::updateParameters()** reads the struct on the audio thread
4. UI components use APVTS attachments (`SliderAttachment`, `ComboBoxAttachment`, `ButtonAttachment`) for bidirectional binding

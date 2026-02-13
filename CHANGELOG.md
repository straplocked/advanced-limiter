# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/), and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Changed
- Open source release under GPL-3.0-or-later

## [0.1.0] - 2026-02-12

### Added
- 8 limiter styles: Transparent, Punchy, Dynamic, Allround, Modern, Aggressive, Bus, Safe
- True peak limiting with 4x oversampled inter-sample peak detection
- Lookahead up to 5 ms with automatic latency compensation
- Input gain vertical fader (-30 to +30 dB) and output level/ceiling control (-30 to 0 dB)
- Stereo channel linking with independent Transient Link and Release Link (0-100%)
- Oversampling up to 32x (1x, 2x, 4x, 8x, 16x, 32x)
- Dithering with target bit depth (16/18/20/22/24) and noise shaping (Off, Basic, Optimized, Weighted)
- DC filter (5 Hz high-pass)
- Loudness metering: Momentary, Short-term, Integrated LUFS, and LRA per EBU R128
- Loudness target indicators (-23, -16, -14 LUFS)
- Real-time scrolling waveform display with input, output, and gain reduction
- 5 display modes and 6 meter scales (including K-system)
- Level meters with peak hold and true peak markers
- Undo/redo with full parameter history
- A/B comparison with instant parameter snapshot switching
- Delta listen mode
- Unity gain monitoring mode
- Audition mode for hearing limited signal
- External sidechain input
- MIDI learn for continuous parameters
- 16 factory presets across Mastering, Mixing, Broadcast, and Creative categories
- Preset browser with search, category filter, and favorites
- Hover help tooltips for all controls
- 23 automatable parameters
- VST3, AU, and Standalone formats (CLAP and AAX optional)

[Unreleased]: https://github.com/straplocked/advanced-limiter/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/straplocked/advanced-limiter/releases/tag/v0.1.0

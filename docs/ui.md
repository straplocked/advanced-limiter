# UI Architecture

## Layout

The UI uses a fixed-ratio resizable layout with the following structure:

```
┌──────────────────────────────────────────────────────┐
│  HeaderToolbar (40 px)                               │
│  Title | Undo/Redo | A/B | Presets | Delta | MIDI    │
├──────┬───────────────────────────────────┬───────────┤
│Input │                                   │ Level     │
│Gain  │  LimiterDisplay                   │ Meter     │
│Strip │  (scrolling waveform)             │ Component │
│(50px)│                                   │ (110px)   │
│      │  ┌──────────────────────────────┐ │           │
│      │  │ LoudnessPanel (33px overlay) │ │           │
│      │  └──────────────────────────────┘ │           │
├──────┴───────────────────────────────────┴───────────┤
│  BottomControlStrip (154 px)                         │
│  Row 1: Style | LA | Atk | Rel | TLink | RLink | Out│
│  Row 2: [TP DC UG Aud SC]  |  [Dither | Combos]     │
└──────────────────────────────────────────────────────┘
```

Default size: 1056 x 726 px. Resizable from 946x545 to 1980x1485.

## Components

### HeaderToolbar
Top bar with plugin title, preset navigation (prev/next/name/save), undo/redo, A/B comparison with copy, delta listen toggle, and MIDI learn button.

### InputGainStrip
Narrow vertical strip on the left side spanning the display height. Contains a `LinearVertical` slider attached to Input Gain (-30 to +30 dB), with a "GAIN" label and dB value readout. Custom `drawLinearSlider` rendering with orange fill from center (0 dB) to current position.

### LimiterDisplay
Main scrolling waveform display showing real-time input level (green), output level (lime), and gain reduction (orange fill from 0 dB downward). Features a dB grid with labeled axis, 0 dB ceiling reference line, and GR peak value labels with marker dots.

### LevelMeterComponent
5-bar vertical meter on the right side: Input L/R, Output L/R, and Gain Reduction. Gradient fill (green to yellow to red), peak hold indicators, true peak markers (red triangles), smoothed values with attack/decay, and a GR value readout at the bottom.

### LoudnessPanel
Semi-transparent overlay at the bottom of the display showing Momentary (M), Short-term (S), Integrated (I) LUFS, and LRA. Includes a loudness target indicator with color-coded dot (green = on target, red = over, blue = under) and a reset button.

### BottomControlStrip
Two-row control panel:
- **Row 1** (large knobs): Style combo, Lookahead, Attack, Release, Transient Link, Release Link, Output Level knobs, Display combo
- **Row 2** (compact): Toggle buttons (TP, DC, UG, Aud, SC) left-aligned, dither controls and combos (Bits, Shape, OS, Meter) on the right

### PresetBrowserPanel
Overlay panel on the right side of the display. Search box, category dropdown filter, favorites toggle, scrollable list with name/category/tags, heart icons for favorites, context menu for favorite/delete, keyboard support (ESC/Enter).

### HelpOverlay
Transparent overlay covering the entire editor. Monitors mouse hover on components with IDs, shows contextual help text after 800ms delay in a frosted-glass tooltip. Help text stored in `HelpTextDatabase`.

## Look and Feel

`AdvancedLimiterLookAndFeel` provides custom rendering for:

- **Rotary sliders**: Gradient body, rim highlight, track arc, orange value arc, pointer dot, notch marks
- **Linear sliders**: Dark groove, orange fill from 0 dB center, bright thumb line, dB tick marks
- **Buttons**: Gradient background with 3D bevel, hover/press states
- **Combo boxes**: Gradient background with arrow indicator
- **Icon buttons**: Custom vector drawing for triangles, save, undo, redo, heart icons

## Colour Palette

Defined in `ColourPalette.h`:

| Colour | Usage |
|--------|-------|
| Orange (`#ff6633`) | Gain reduction, knob indicator, accent |
| Green (`#3a9a3a`) | Input level |
| Lime (`#7aff3a`) | Output level |
| Red (`#ff4444`) | Ceiling line, true peak marker |
| Yellow (`#ffcc00`) | GR peak labels |
| Blue (`#44aaff`) | Loudness target |
| Dark backgrounds | `#040408` to `#0e0e18` |

## Font System

Two typefaces loaded from binary data:
- **Space Grotesk** (body text, bold): Used for labels, values, buttons
- **Audiowide** (title): Used for plugin title

All fonts are scaled by `Fonts::kScale = 1.1f` for readability.

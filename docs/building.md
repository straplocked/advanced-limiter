# Building

## Requirements

| Requirement | Version |
|-------------|---------|
| CMake | 3.25+ |
| C++ Standard | C++20 |
| JUCE | 7.0.12 (included as submodule) |

### Platform-Specific

**macOS**: Xcode Command Line Tools or Xcode IDE. Deployment target: macOS 10.13+. Builds universal binary (arm64 + x86_64).

**Windows**: Visual Studio 2022 with C++ workload.

**Linux**: GCC 12+ or Clang 14+. Additional packages: `libasound2-dev`, `libfreetype6-dev`, `libx11-dev`, `libxrandr-dev`, `libxcursor-dev`, `libxinerama-dev`.

## Quick Start

```bash
git clone --recurse-submodules https://github.com/straplocked/advanced-limiter.git
cd advanced-limiter
cmake -B build
cmake --build build
```

## Build Configuration

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `ADVANCED_LIMITER_BUILD_TESTS` | OFF | Build test suite |
| `AAX_SDK_PATH` | (unset) | Path to AAX SDK for Pro Tools format |
| `CLAP_JUCE_EXTENSIONS_DIR` | `libs/clap-juce-extensions` | Path to CLAP extensions |

### Plugin Formats

Formats are selected automatically based on platform and available SDKs:

| Format | Condition |
|--------|-----------|
| Standalone | Always |
| VST3 | Always |
| AU | macOS only |
| CLAP | If `libs/clap-juce-extensions` submodule present |
| AAX | If `AAX_SDK_PATH` is set |

### Compiler Flags

- `-ffast-math` (GCC/Clang) or `/fp:fast` (MSVC) for DSP performance
- JUCE recommended config, LTO, and warning flags applied automatically

### Preprocessor Definitions

| Define | Value | Purpose |
|--------|-------|---------|
| `JUCE_WEB_BROWSER` | 0 | Disable web browser component |
| `JUCE_USE_CURL` | 0 | Disable curl networking |
| `JUCE_VST3_CAN_REPLACE_VST2` | 0 | Don't replace VST2 plugins |
| `JUCE_DISPLAY_SPLASH_SCREEN` | 0 | No splash screen |
| `ADVANCED_LIMITER_VERSION` | "0.1.0" | Version string |

## Build Artifacts

After building, plugins are located in `build/AdvancedLimiter_artefacts/`:

```
build/AdvancedLimiter_artefacts/
├── Standalone/
│   └── Advanced Limiter.app          (macOS)
├── VST3/
│   └── Advanced Limiter.vst3
└── AU/
    └── Advanced Limiter.component    (macOS only)
```

With `COPY_PLUGIN_AFTER_BUILD` enabled (default), plugins are automatically installed to system plugin directories:

| Format | macOS Install Path |
|--------|--------------------|
| AU | `~/Library/Audio/Plug-Ins/Components/` |
| VST3 | `~/Library/Audio/Plug-Ins/VST3/` |

## CLAP Support

CLAP format requires the `clap-juce-extensions` submodule:

```bash
git submodule update --init libs/clap-juce-extensions
cmake -B build
cmake --build build
```

CLAP features: `audio-effect`, `limiter`, `dynamics`, `mastering`.

## AAX Support (Pro Tools)

Requires the Avid AAX SDK (not publicly distributed):

```bash
cmake -B build -DAAX_SDK_PATH=/path/to/aax-sdk
cmake --build build
```

## Release Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

JUCE recommended LTO flags are applied automatically.

## Troubleshooting

**"JUCE not found"**: Ensure submodules are initialized: `git submodule update --init --recursive`

**AU not loading in DAW**: The AU is ad-hoc signed during build. For distribution, code-sign with a Developer ID certificate.

**High CPU usage**: Try increasing buffer size in your DAW, or reduce oversampling from 32x to a lower setting.

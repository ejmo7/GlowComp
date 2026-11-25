# GlowComp

GlowComp is a compact stereo/mono compressor VST3 plugin built with JUCE. It provides threshold, ratio, attack, release, makeup gain, and wet/dry mix controls, with a custom dark interface.

## Build

Requirements: CMake 3.22+, a C++17 compiler, Git, and the platform build tools required by JUCE. CMake fetches the pinned JUCE 8.0.6 source on the first configure.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The VST3 bundle is generated under `build/GlowComp_artefacts/Release/VST3` (or the corresponding configuration directory for the chosen generator). The standalone app is built alongside it. Install the VST3 bundle in your host's VST3 plug-in directory and rescan plug-ins.

## Controls

- **Threshold:** -60 to 0 dB
- **Ratio:** 1:1 to 20:1
- **Attack:** 0.1 to 200 ms
- **Release:** 10 to 1000 ms
- **Makeup:** 0 to 24 dB
- **Mix:** dry-to-wet blend

Parameter values are saved and restored with the host's session state.

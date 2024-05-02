# microbiome
<p align="center">
  <img src="https://github.com/dsmaugy/microbiome/assets/8429648/2319ad61-b687-4ecc-9b1f-b8c4e8a0c4fb">
</p>

Microbiome is a real-time audio effects plugin that uses delay lines and looping to create vibrant audio artifacts mimicking the nature of evolutionary biology.

The main Microbiome audio processing engine is done through a variable number of sound "Colonies" that can be toggled on/off at will.
Each Colony resamples a user-defined section of a global delay line, feeding its processed output back to the output signal.

Microbiome has built-in reverb/compression/LPF, various re-sampling methods per colony, a plugin editor UI, and supports parameter automation. 

## Compatibility
Microbiome can be downloaded as an AU plugin on MacOS, VST3 on Windows, and VST3/LV2 on Linux.
See [Installation](#Installation) for installation steps.

## Video Demo
[![Video Demo](https://img.youtube.com/vi/Ihh65Blad9o/0.jpg)](https://www.youtube.com/watch?v=Ihh65Blad9o)

## Installation

### Windows
- Download the latest **Microbiome_Win_VST3** zip from the [releases page](https://github.com/dsmaugy/microbiome/releases)
- Open the zip and extract the `Microbiome.vst3` directory to your VST3 folder (probably `C:\Program Files\Common Files\VST3`)

### MacOS (last built for macOS Monterey version 12.3)
- Download the latest **Microbiome_MacOS_AU** zip from the [releases page](https://github.com/dsmaugy/microbiome/releases)
- Open the zip and extract the `Microbiome.component` directory `/Library/Audio/Plug-Ins/Components`
- If that does not work, the AU can also be installed under `Users/<your username>/Library/Audio/Plug-Ins/Components`

### Linux

#### VST3
- Download the latest **Microbiome_Linux_x86_VST3** zip from the [releases page](https://github.com/dsmaugy/microbiome/releases)
- Open the zip and extract the `Microbiome.vst3` directory to `~/.vst3/`

#### LV2
- Download the latest **Microbiome_Linux_x86_LV2** zip from the latest [releases page](https://github.com/dsmaugy/microbiome/releases)
- Open the zip and extract the `Microbiome.lv2` directory to `~/.lv2/`

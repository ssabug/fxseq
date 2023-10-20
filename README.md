# Fxseq

## Description
Fxseq is a step sequencer-based multi effect like dblue glitch or effectrix.

![alt text](Ressources/images/GUI.png)

see ![DOCUMENTATION](DOCUMENTATION.md) for more details.

## Requirements
 - JUCE Version 7.0.7
 - gcc / mingw32-gcc-posix (Linux)
 - MSVC compiler / mingw (Windows)

## Installation
### Linux
 - get the pre-built **.vst3** folder in **Releases** or build it (see below).
 - put it in your VST3 folder
 - run the script `./Ressources/scripts/copy_plugin_data_linux.sh`

It will create the folders and copy the plugin data in **${HOME}/.ssabug/fxseq**
### Windows
 - get the pre-built **.vst3** folder in **Releases** or build it (see below).
 - put it in your VST3 folder
 - run the script `.\Ressources\scripts\copy_plugin_data_windows.cmd`

It will create the folders and copy the plugin data in **%HomePath%\ssabug\fxseq**
## Building 
<details>
  <summary> ### Linux </summary>

 - git clone the repo or get a release zip. Extract if necessary.
 - in the created folder `fxseq`, open the file **fxseq.jucer** with JUCE Projucer
 - generate the project for your IDE or Linux Makefile
 - build in a IDE or, in a terminal, run `cd Builds/LinuxMakefile/ && make`
 - if manually built, the VST3 folder will be in the  **Builds/LinuxMakefile/build** directory
 </details>
 ### Windows
 - git clone the repo or get a release zip. Extract if necessary.
 - in the created folder `fxseq`, open the file **fxseq.jucer** with JUCE Projucer
 - generate the project for your IDE and build

## Compatibility
| OS            | Working               |  Not working          |
|:-------------:|:---------------------:|:---------------------:|
| Linux         | Bitwig                | Qtractor              |
|               | Tracktion Waveform 12 | Carla                 |
|               |                       | Ardour                |
| Windows       | Bitwig                |                       |
|               | Ableton 11            |                       |

Linux distributions tested : Fedora, Arch Linux

Windows versions tested : 11

## History
see ![HISTORY](HISTORY.md) for more details.

# Fxseq

## Description
Fxseq is a step sequencer-based multi effect like dblue glitch or effectrix.

![alt text](Ressources/images/GUI.png)

see ![DOCUMENTATION](DOCUMENTATION.md) for more details.

## Requirements
 - JUCE Version 7.0.7
 - gcc

## Installation
### Linux
 - get the pre-built **.vst3** folder in **Releases** or build it (see below).
 - put it in your VST3 folder
 - run the script `./Ressources/scripts/copy_plugin_data.sh [path to downloaded github stuff]`

It will create the folders and copy the plugin data in **${HOME}/.ssabug/fxseq/**

You're ready to go.

## Building 
### Linux
 - git clone the repo or get a release zip. Extract if necessary.
 - Then `cd fxseq`  
 - Run the build script with `./Ressources/scripts/build_linux_vst3.sh "$(pwd)"`
 - The script will ask for your **JUCE/modules** path
 - the VST3 folder will be in the  **Builds/LinuxMakefile/build/fxseq.vst3** directory

Tested OSs  : Fedora

Tested DAWs : Bitwig

## History
see ![HISTORY](HISTORY.md) for more details.

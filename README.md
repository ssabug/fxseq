# Fxseq


## Description
Fxseq is a step sequencer-based multi effect unlike dblue glitch or effectrix.

![alt text](Ressources/images/GUI.png)

###Sequencers
Each effect is triggered by its own sequencer, for which you can change clock, selected pattern and position (using the two left up/down circle buttons).

Steps on the sequencer can either be two-state (on/off) or multi-state ( 1/2/3/4 or enveloppe selection) , depending on the effect.

You can copy/paste/clear/reset each sequencer pattern using the 4 buttons + combobox in the **Options** sections.

###Sequence sequencer
You can define sequences of patterns to be played.

Each step combobox represent a pattern to be selected on all sequencers.

You can switch between sequences using the left combobox.

**Sequence length** changes the length of the sequence. 

###Effects
There are 4 effects : chopper, echo (provided with faust examples), ladder 12dB filter & bitcrusher.

The bottom left section displays all available effect parameters. For effects supporting multi-state, theres a top left button to switch between each state.

Each defined multi state can then be triggered with the corresponding 1/2/3/4 steps on the sequencer.

###Options
Presets are callable using the **Presets** combobox.

You can create a new preset by changing the text in the combobox and click on the **Save** button.

**Mode** changes the sequencers mode to sequence or pattern. 

In **pattern** mode, the sequencers play in loop their respective selected pattern.

In **sequence** mode, the sequencers play the patterns defined in the current selected **sequence**.

**Autoscroll** enables/disables pattern display switching while in **sequence** mode.

**Copy,paste,clear,reset** buttons modify accordingly the sequencer selected with the left combobox.

###Output
**Out gain** trims the output level.

**Dry/wet** blends between dry input and processed signals.

## Requirements
JUCE Version 7.0.7

gcc

## Installation
### Linux
 - get the pre-built **.vst3** folder in **Releases** or build it (see below).
 - put it in your VST3 folder
 - run the script `./Ressources/scripts/copy_plugin_data.sh [path to downloaded github stuff]`

It will create the folders and copy the plugin data in **${HOME}/.ssabug/fxseq/**

 - load the plugin in your DAW

## Building 
### Linux
 - git clone the repo or get a release zip. Extract if necessary.
 - Then `cd fxseq`  
 - Run the build script with `./Ressources/scripts/build_linux_vst3.sh "$(pwd)"`
 - The script will ask for your **JUCE/modules** path
 - the VST3 folder will be in the  **Builds/LinuxMakefile/build/fxseq.vst3** directory

Tested OSs  : Fedora

Tested DAWs : Bitwig

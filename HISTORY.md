# Fxseq

## Change records
### v0.1 27102023  
#### TODO
- [x] Call install scripts from Cmake (auto)
- [x] Pass variable value while building cmake on windows/linux
- [x] update build scripts & install documentation
- [ ] change default directories?? (linux .local/share, windows Documents\)
- [ ] store current skin
- [ ] add program parameters to combFilter
- [ ] in PluginEditor, find a way to acquire plugin parameters attributes
- [ ] betatest pluginParameters values loading at editor start **(and processor start as well)** 
- [ ] implement TimeStretcher.h
- [x] make linux LV2 working (more or less...)
- [ ] Compatibility : find out why Windows VST3 doesnt work in some cases
- [ ] add more effects : fix ring modulator (**sounds strange**) / convolution verb 
- [ ] cleaner way to pass colortemplates & skin/image paths
- [ ] in parameter layout constructor find a way to destroy created effects objects
#### BUGS
- [ ] program parameters control does not update on GUI after loading presets
- [ ] strange behaviour while moving effect's position
- [ ] effect position not restored on preset load (works with DAW save)
- [ ] buffer seems wrongly initialised on press play(sequence mode)
- [ ] latency (same problem as choppah) -> maybe due to chopper gain ramp

### v0.1 17102023  
#### TODO
- [x] put all effects in separate .h files
- [x] tidy source files arbo
- [x] Linux / Windows / Mac code & path compatibility (**TO BE TESTED**)
- [x] clean unused pluginProcessor variables
- [x] implement load/save programParameters from XML presets (**TO BE TESTED**)
- [x] Cmake implementation start : Linux built OK, Windows Built OK, **mingw32-g++-posix vst3s do not work**
- [x] pitch shifter (**freq sweep to be improved**) / combFilter fix
- [x] create windows installation script
- [ ] Call install scripts from Cmake
- [ ] change default directories?? (linux .local/share, windows Documents\)
- [ ] store current skin
- [ ] add program parameters to combFilter
- [ ] in PluginEditor, find a way to acquire plugin parameters attributes
- [ ] betatest pluginParameters values loading at editor start **(and processor start as well)** 
- [ ] implement TimeStretcher.h
- [x] make linux LV2 working
- [ ] Compatibility : find out why Windows VST3 doesnt work in some cases
- [ ] add more effects : fix ring modulator (**sounds strange**) / convolution verb 
- [ ] cleaner way to pass colortemplates & skin/image paths
- [ ] in parameter layout constructor find a way to destroy created effects objects
#### BUGS
- [x] pitchshifter clicks
- [x] combFilter clicks
- [x] echo step on->off clicks
- [x] wet output is mono (was combfilter)
- [x] effects gain should not apply to dry signal
- [x] crashes in sequence mode (actually because some default patterns included values > 1 for non program supporting fx)  
- [x] dry/wet for filter cracks
- [ ] program parameters control does not update on GUI after loading presets
- [ ] strange behaviour while moving effect's position
- [ ] effect position not restored on preset load (works with DAW save)
- [ ] buffer seems wrongly initialised on press play(sequence mode)
- [ ] latency (same problem as choppah) -> maybe due to chopper gain ramp

### v0.05 29092023  
#### TODO
- [x] switch to 8 fx + GUI resize
- [x] load all pluginParameters value at editor start **to be really betatested**
- [x] implement load/save XML presets/**config + programParameters TBD**
- [x] write get/Set state information for all processor parameters & patterns & sequences
- [x] added distortion / ring modulator (**sounds strange**) / pitch shifter (**freq sweep to be improved**) / combFilter (**doesnt work**)
- [ ] add more effects :  convolution verb / stretcher /  **repeater is a miss for now**
- [ ] cleaner way to pass colortemplates & skin/image paths
- [ ] clean or rename FaustEchoEffect.h because other FAUST effects (ringmod + combfilter + pitch shifter) had been put into & remove unused faust
#### BUGS
- [ ] pitchshifter clicks
- [ ] effects gain should not apply to dry signal **partly corrected**
- [ ] effect position not restored on preset load (works with DAW save)
- [x] saved presets are corrupted (empty pattern/sequence fields)
- [ ] crashes in sequence mode
- [ ] dry/wet for filter cracks
- [ ] step on->off clicks for echo
- [x] crusher clicks 
- [ ] buffer seems wrongly initialised on press play(sequence mode)
- [ ] latency (same problem as choppah) -> maybe due to chopper gain ramp

### v0.04 25092023  
#### TODO
- [x] processor audio parameters definition cleaned & updated
- [x] added listeners to GUI
- [x] load all pluginParameters value at editor start **to be really betatested**
- [ ] implement load/save XML presets/config (**done for audioparameters**)
- [x] write get/Set state information for all processor parameters
- [ ] add more effects  **repeater is a miss for now**
- [ ] cleaner way to pass colortemplates & skin/image paths
#### BUGS
- [ ] some crashes in sequence mode while hiding/showing GUI
- [ ] dry/wet for filter cracks
- [ ] step on->off clicks for echo
- [ ] crusher clicks 
- [ ] buffer seems wrongly initialised on press play(sequence mode)
- [ ] latency (same problem as choppah) -> maybe due to gain ramp

### v0.03 25092023
#### TODO
- [x] define processor fx parameters **not all used tbc**
- [x] put all effect in functions 
- [x] implement master dry/wet & gain
- [x] implement fx gains
- [x] implement bitcrush part of bitcrusher
- [x] add utility copy/paste/clear/reset buttons  
- [x] load all pluginParameters value at editor start (clockDiv,fx,....)
- [ ] implement load/save XML presets
- [ ] write get/Set state information for all processor parameters
- [x] continue definition of EffectComponent & OptionsComponent
- [ ] add more effects  **fine tuning TBD**
- [x] resize sequencers' effect name comboboxes to 1 item
- [ ] cleaner way to pass colortemplates & skin/image paths
#### BUGS
- [x] effectComponent : trying to get param value at object creation crashes => fix : move slider value update to resize()
- [ ] dry/wet for filter cracks
- [ ] step on->off clicks for echo
- [ ] crusher clicks 
- [ ] buffer seems wrongly initialised on press play(sequence mode)

### v0.03 24092023
#### TODO
- [ ] define processor fx parameters
- [ ] load all pluginParameters value at editor start (clockDiv,fx,....)
- [ ] implement load/save XML presets
- [ ] write get/Set state information for all processor parameters
- [ ] continue definition of EffectComponent & OptionsComponent
- [x] add more effects : echo, filter **fine tuning TBD**
- [x] SequenceSequencerComponent fully implemented
- [x] reimplement chopper
- [x] update processor gainpPatterns while modifying patterns on GUI (**crashes**)
- [x] implement sequencer position change ability 
- [ ] cleaner way to pass colortemplates
#### BUGS

### v0.02 22092023
#### TODO
- [x] define all processor parameters => **all but effect specific fx parameters mapped**
- [ ] write get/Set state information for all processor parameters
- [x] continue definition of EffectComponent & OptionsComponent
- [x] define SequenceSequencerComponent 
- [ ] update processor gainpPatterns while modifying patterns on GUI (**crashes**)
- [ ] cleaner way to pass colortemplates
#### BUGS
- [x] setting the textbutton label color(savePresetButton in options component object) make it crash (why???)

### v0.01 21092023 Init project
#### TODO
- [ ] define all processor parameters
- [ ] continue definition of EffectComponent & OptionsComponent
- [ ] define OutputComponent & SequenceComponent
- [ ] update processor gainpPatterns while modifying patterns on GUI (**crashes**)
- [ ] cleaner way to pass colortemplates



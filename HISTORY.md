# Fxseq

## Change records

### v0.03 25092023 Init project
#### TODO
- [ ] define processor fx parameters
- [x] put all effect in functions 
- [ ] implement master dry/wet & gain
- [x] implement bitcrush part of bitcrusher
- [ ] add utility copy/paste/clear/reset buttons  
- [ ] load all pluginParameters value at editor start (clockDiv,fx,....)
- [ ] implement load/save XML presets
- [ ] write get/Set state information for all processor parameters
- [ ] continue definition of EffectComponent & OptionsComponent
- [ ] add more effects  **fine tuning TBD**
- [ ] cleaner way to pass colortemplates
#### BUGS
- [x] effectComponent : trying to get param value at object creation crashes => fix : move slider value update to resize()
- [ ] dry/wet for filter cracks
- [ ] step on->off clicks for echo
- [ ] crusher clicks 
- [ ] buffer seems wrongly initialised on press play(sequence mode)

### v0.03 24092023 Init project
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

### v0.02 22092023 Init project
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



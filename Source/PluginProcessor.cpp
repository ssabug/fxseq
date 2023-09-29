/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "FaustEchoEffect.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    std::string fxNamesStr[4]={"Chopper","Echo","Filter","Crusher"};
 
    params.add (std::make_unique<juce::AudioParameterFloat> ("drywet","Dry/Wet", 0.0f, 1.0f, 1.0f));
    params.add (std::make_unique<juce::AudioParameterFloat> ("outgain","Out Gain",0.0f, 2.0f, 1.0f));

    params.add (std::make_unique<juce::AudioParameterBool> ("sequencerMode","sequencerMode", false));
    params.add (std::make_unique<juce::AudioParameterInt> ("sequenceNumber","sequenceNumber", 1, 16, 1));
    params.add (std::make_unique<juce::AudioParameterInt> ("sequenceLength","sequenceLength", 2, 16, 4));

    for (int i = 1; i < 4+1; ++i)
    {
        std::string prefix=fxNamesStr[i-1]+"_";
        params.add (std::make_unique<juce::AudioParameterInt> (prefix+"pattern",prefix+"pattern", 1, 16, 1));
        params.add (std::make_unique<juce::AudioParameterChoice> (prefix+"clockDiv",prefix+"clockDiv",juce::StringArray({ "4","2","1","1/2","1/4","1/8","1/16","1/32" }), 2));
        params.add (std::make_unique<juce::AudioParameterInt> (prefix+"position",prefix+"position",0, 3, 1)); 
    }

    std::string prefx=fxNamesStr[0]+"_";//chopper
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"gain",prefx+"gain", 0.0f, 2.0f, 0.1f));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"dry/wet",prefx+"dry/wet", 0.0f, 1.0f, 0.1f));         
    
    prefx=fxNamesStr[1]+"_";//echo
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"time",prefx+"time", 0.0f, 1.0f, 0.1f));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"feedback",prefx+"feedback", 0.0f, 1.0f, 0.1f));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"gain",prefx+"gain", 0.0f, 2.0f, 0.1f));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"dry/wet",prefx+"dry/wet", 0.0f, 1.0f, 0.1f)); 

    prefx=fxNamesStr[2]+"_";//filter
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"frequency",prefx+"frequency", 20.0f, 20000.0f, 10.0f));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"resonance",prefx+"resonance", 0.0f, 1.0f, 0.1f));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"drive",prefx+"drive", 1.0f, 10.0f, 0.1f));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"gain",prefx+"gain", 0.0f, 2.0f, 0.1f));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"dry/wet",prefx+"dry/wet", 0.0f, 1.0f, 0.1f)); 

    prefx=fxNamesStr[3]+"_";//crusher
    params.add (std::make_unique<juce::AudioParameterInt> (prefx+"reduction",prefx+"reduction", 0, 30, 1));
    params.add (std::make_unique<juce::AudioParameterInt> (prefx+"bitdepth",prefx+"bitdepth", 0, 32, 1));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"gain",prefx+"gain", 0.0f, 2.0f, 0.1f));
    params.add (std::make_unique<juce::AudioParameterFloat> (prefx+"dry/wet",prefx+"dry/wet", 0.0f, 1.0f, 0.1f)); 

    return params;
}
//==============================================================================
FxseqAudioProcessor::FxseqAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)                     
                                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),  
       pluginParameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    initAllPatterns();
    
    filter.reset();
    //filter.setMode(juce::dsp::LadderFilter<float>::Mode::LowPass);
    filter.setCutoffFrequencyHz(20000.0f);
    filter.setResonance(0.0f);

    fx1dryWetMixer.setWetMixProportion(1.0f);fx1dryWetMixer.setWetLatency(0.0f);
    fx2dryWetMixer.setWetMixProportion(1.0f);fx2dryWetMixer.setWetLatency(0.0f);
    fx3dryWetMixer.setWetMixProportion(1.0f);fx3dryWetMixer.setWetLatency(0.0f);
    fx4dryWetMixer.setWetMixProportion(1.0f);fx4dryWetMixer.setWetLatency(0.0f);

    for (int i=0;i<sequencerCount;i++)
    {
        updateParameter(fxNamesStr[i]+"_dry/wet",1.0f);
        updateParameter(fxNamesStr[i]+"_gain",1.0f);
        updateParameter(fxNamesStr[i]+"_position",(float) i);
    }
    updateParameter("sequencerMode",(float)0);
}

FxseqAudioProcessor::~FxseqAudioProcessor()
{

}
//==============================================================================
const juce::String FxseqAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FxseqAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FxseqAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FxseqAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FxseqAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FxseqAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FxseqAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FxseqAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FxseqAudioProcessor::getProgramName (int index)
{
    return {};
}

void FxseqAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FxseqAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{   
    // FAUST ECHO
    echoDSP = new mydsp();
    echoDSP->init(sampleRate);
    echoUI = new MapUI();
    echoDSP->buildUserInterface(echoUI);
    echoInputs = new float*[2];
    echoOutputs = new float*[2];
    for (int channel = 0; channel < 2; ++channel) {
        echoInputs[channel] = new float[samplesPerBlock];
        echoOutputs[channel] = new float[samplesPerBlock];
    }

    filter.reset();

    fx1drybuffer.setSize(2, samplesPerBlock);
    fx2drybuffer.setSize(2, samplesPerBlock);
    fx3drybuffer.setSize(2, samplesPerBlock);
    fx4dryBuffer.setSize(2, samplesPerBlock); 

    masterDryBuffer.setSize(2, samplesPerBlock); 

    repeater_buffer.clear();
    buffer_pos=0;

    //filter.setSampleRate(sampleRate);
    //filter.prepare({ sampleRate, (double)maximumBlockSize, (double)getTotalNumInputChannels() });
}

void FxseqAudioProcessor::releaseResources()
{
    // FAUST ECHO
    delete echoDSP;
    delete echoUI;
    for (int channel = 0; channel < 2; ++channel) {
        delete[] echoInputs[channel];
        delete[] echoOutputs[channel];
    }
    delete [] echoInputs;
    delete [] echoOutputs;
    repeater_buffer.clear();
    buffer_pos=0;
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FxseqAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FxseqAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::AudioPlayHead* phead = getPlayHead();
    if (phead != nullptr) // Compute positions on patterns/sequence 
    {
        auto playposinfo=phead->getPosition(); // get DAW play position
        ppq=*(*playposinfo).getPpqPosition(); // get DAW ppq
        bpm=*(*playposinfo).getBpm();
        isPlaying=(*playposinfo).getIsPlaying();
        
        for (int i=0;i<sequencerCount;i++) // FOR EACH SEQUENCER
        {   
            sequencerClockMult[i]=(int)pow(2,(pluginParameters.getParameter(fxNamesStr[i] + "_clockDiv")->getValue()*7));    // get clock mult 
            sequencerPositions[i]=int(std::floor( std::modf(ppq/sequencerClockMult[i],&unused)*(16*resolution))); // compute position on pattern
            if (not (bool)pluginParameters.getParameter("sequencerMode")->getValue()){  // PATTERN MODE          
                selected_pattern[i]=(int)getParameterValue(fxNamesStr[i] + "_pattern"); //get selected pattern
            } else {                                                                   // SEQUENCE MODE
                int selectedSequence=(int)(pluginParameters.getParameter("sequenceNumber")->getValue()*15); // get selected sequence                
                int sequencePosition;
                sequenceLength=(int)getParameterValue("sequenceLength")+2;
                if (ppq >= (float)sequenceLength ) {sequencePosition=int(std::floor(ppq/greatestClockMult)-sequenceLength*std::floor(ppq/(sequenceLength*greatestClockMult)));} else {sequenceLength=std::floor(ppq/greatestClockMult);}         // compute positon on sequence            
                
                sequencePosition=std::clamp(sequencePosition,0,15); // safety 
                selected_pattern[i]=sequences[selectedSequence][sequencePosition]; // update selected pattern
            }
            lastFxDepths[i]=fxDepths_smoothed[i].getNextValue(); // get smoothed value from previous value
            fxDepths_smoothed[i].setTargetValue(gainPatterns[i][selected_pattern[i]][sequencerPositions[i]]);// get data from pattern and set it for smoothed value target            
        }
        
       
    }

    float outMix=getParameterValue("drywet");
    float outGain=getParameterValue("outgain");
    float dry=(1.0f-outMix)*outGain,wet=outMix*outGain;

    for (int channel = 0; channel < totalNumInputChannels; ++channel) 
    {
        masterDryBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());// copy dry input 
    }
	
    for (int i=0;i<sequencerCount;i++)
    {
        switch (fxPositions[i])
        {
            case 0:
               chopper_process(buffer,fx1drybuffer); 
               break;
            case 1: 
                echo_process(buffer);
                break;
            case 2:
                filter_process(buffer,fx3drybuffer);
                //repeater_process(buffer,fx3drybuffer);
                break;
            case 3:               
                bitcrush_process(buffer);
                break;
            default:
                break;
        }
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel) 
    {
        juce::FloatVectorOperations::multiply(masterDryBuffer.getWritePointer(channel), dry, buffer.getNumSamples()); // multiply dry buffer by gain * 1-wetvolume
        juce::FloatVectorOperations::multiply(buffer.getWritePointer(channel), wet, buffer.getNumSamples()); // multiply wet buffer by gain * wetvolume
	    juce::FloatVectorOperations::add(buffer.getWritePointer(channel), masterDryBuffer.getReadPointer(channel), buffer.getNumSamples());  // add dry to wet   
    } 
 
}

//==============================================================================
bool FxseqAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FxseqAudioProcessor::createEditor()
{
    return new FxseqAudioProcessorEditor (*this);
}

//==============================================================================
void FxseqAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{   
    auto state = pluginParameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    juce::XmlElement* patternDef=getAllPatternsXml();
    xml->addChildElement(patternDef);
    copyXmlToBinary (*xml, destData);
}

void FxseqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (pluginParameters.state.getType()))
            pluginParameters.replaceState (juce::ValueTree::fromXml (*xmlState));
            //loadPatternsAndSequencesFromXML(xmlState.get());
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FxseqAudioProcessor();
}

void FxseqAudioProcessor::initAllPatterns()
{
    gainPatterns.clear();
    for (int i=0;i<sequencerCount;i++) 
    {
        std::vector<std::vector<float>> gainPatts;
        for (int j=0;j<patternsPerSequencer;j++)
        {
            
            gainPatts.push_back(generateGainPattern(i,j));
        }
        gainPatterns.push_back(gainPatts);
    }
}

void FxseqAudioProcessor::updateGainPattern(int sequencerIndex,int patternIndex)
{
    gainPatterns[sequencerIndex][patternIndex]=generateGainPattern(sequencerIndex,patternIndex);
}

std::vector<float> FxseqAudioProcessor::generateGainPattern(int sequencerIndex,int patternIndex)
{
    std::vector<float> outVector;    
    int value;

   for (int l=0;l<stepsPerPattern;l++) 
    {
        value=patterns[sequencerIndex][patternIndex][l];     
        for(int i=0;i<resolution;i++)
        {    
            if (value > 0) {
                outVector.push_back(enveloppes[value-1][i]);
            } else {
                outVector.push_back(0.0);
            }
        }
    }        
    return outVector;
}

void FxseqAudioProcessor::updateParameter(std::string paramName,float paramValue)
{
    auto rg=pluginParameters.getParameter(paramName)->getNormalisableRange().end-pluginParameters.getParameter(paramName)->getNormalisableRange().start;
    pluginParameters.getParameter(paramName)->setValue(paramValue/rg);
}

float FxseqAudioProcessor::getParameterValue(std::string paramName)
{
    auto rg=pluginParameters.getParameter(paramName)->getNormalisableRange().end-pluginParameters.getParameter(paramName)->getNormalisableRange().start;
    return pluginParameters.getParameter(paramName)->getValue()*rg;
}

void FxseqAudioProcessor::updateEffectProgramParameter(int effectIndex,int programIndex,int parameterIndex,float parameterValue)
{
    fxPrograms[effectIndex][programIndex][parameterIndex]=parameterValue;
}

float FxseqAudioProcessor::getEffectProgramParameterValue(int fxIndex,int programIndex,int paramIndex)
{
    //debug="fxIndex " + std::__cxx11::to_string(fxIndex) + " programIndex " + std::__cxx11::to_string(programIndex) + " paramIndex " + std::__cxx11::to_string(paramIndex);
    //return fxPrograms[fxIndex][programIndex][paramIndex];
    return 0.0f;
}
//////////////////////////// FX CALLBACKS ////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FxseqAudioProcessor::chopper_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer)
{
    float fx1mix=getParameterValue("Chopper_dry/wet");
    float gain=getParameterValue("Chopper_gain");


    for (int channel = 0; channel < 2; ++channel) {
        dryBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());// copy dry input 
        buffer.applyGainRamp(channel,0,buffer.getNumSamples(),lastFxDepths[0],fxDepths_smoothed[0].getNextValue());  // chopper         
    }
    float wet = gain * fx1mix;
    float dry = gain *( 1.0f - fx1mix);

    juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(0), dry, buffer.getNumSamples());
	juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(1), dry, buffer.getNumSamples());

	juce::FloatVectorOperations::multiply(buffer.getWritePointer(0), wet, buffer.getNumSamples());
	juce::FloatVectorOperations::multiply(buffer.getWritePointer(1), wet, buffer.getNumSamples());

	juce::FloatVectorOperations::add(buffer.getWritePointer(0), dryBuffer.getReadPointer(0), buffer.getNumSamples());
	juce::FloatVectorOperations::add(buffer.getWritePointer(1), dryBuffer.getReadPointer(1), buffer.getNumSamples());    
}

void FxseqAudioProcessor::echo_process(juce::AudioBuffer<float>& buffer)
{
    float gain=getParameterValue("Echo_gain");
    float fx2mix=getParameterValue("Echo_dry/wet");    
    float echoMix=fxDepths_smoothed[1].getNextValue();

    for (int channel = 0; channel < 2; ++channel) {
        for (int i = 0; i < buffer.getNumSamples(); i++) { 
                echoInputs[channel][i] = *buffer.getWritePointer(channel,i); //Echo
            }
    }
    echoDSP->compute(buffer.getNumSamples(),echoInputs,echoOutputs);
    for (int channel = 0; channel < 2; ++channel) {
        for (int i = 0; i < buffer.getNumSamples(); i++){
            *buffer.getWritePointer(channel,i) =gain*(echoOutputs[channel][i]*echoMix*fx2mix+(1-echoMix*fx2mix) * echoInputs[channel][i]); // echo  
        }
    }
}

void FxseqAudioProcessor::bitcrush_process(juce::AudioBuffer<float>& buffer)
{
    int reduction = int(fxPrograms[3][patterns[3][selected_pattern[3]][int(sequencerPositions[3]/resolution)]][0]);// FX4
    int bitDepth = int(fxPrograms[3][patterns[3][selected_pattern[3]][int(sequencerPositions[3]/resolution)]][1]);// FX4    

    float fx4mix=getParameterValue("Crusher_dry/wet");
    float gain=getParameterValue("Crusher_gain");    
    //float crusherMix=std::clamp(fxDepths_smoothed[3].getNextValue(),0.0f,1.0f);//clamp(fxDepths_smoothed[3].getNextValue(),0,1);
    float crusherMix=std::clamp((float)(patterns[3][selected_pattern[3]][int(sequencerPositions[3]/resolution)]),0.0f,1.00f);
    for (int channel = 0; channel < 2; ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++){

            float totalQLevels =powf(2, bitDepth);
			float val = channelData[i];
            
			channelData[i] =val*(1-crusherMix*fx4mix) + (val - std::fmod(val, 1/totalQLevels )) * crusherMix*fx4mix;//*crusherMix*fx4mix + (1-crusherMix*fx4mix)*val;       // Bit Quantizing      
 
             if (reduction > 1) {if (i%reduction != 0) channelData[i] = crusherMix*fx4mix*channelData[i - i%reduction]+(1-crusherMix*fx4mix)*channelData[i];} // sample rate reduction

            channelData[i] = gain * channelData[i];
        }
    }
}

void FxseqAudioProcessor::filter_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer)
{
    float fx3mix=getParameterValue("Filter_dry/wet");
    float gain=getParameterValue("Filter_gain");
    float frequency = fxPrograms[2][patterns[2][selected_pattern[2]][int(sequencerPositions[2]/resolution)]][0];// FX4
    float resonance = fxPrograms[2][patterns[2][selected_pattern[2]][int(sequencerPositions[2]/resolution)]][1];// FX4 
    float drive=  fxPrograms[2][patterns[2][selected_pattern[2]][int(sequencerPositions[2]/resolution)]][2];
    filterFreq_smoothed.setTargetValue(frequency);

    if (frequency !=lastFilterFrequency ) {
        filter.setCutoffFrequencyHz(filterFreq_smoothed.getNextValue());
        lastFilterFrequency=frequency;
    }
    if (resonance !=lastFilterResonance ) {
        filter.setResonance(resonance);
        lastFilterResonance=resonance;
    }

    if (drive !=lastFilterDrive) {
        filter.setDrive(drive);
        lastFilterDrive=drive;
    }

    dryBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());// copy dry input   
    dryBuffer.copyFrom(1, 1, buffer, 1, 0, buffer.getNumSamples());// copy dry input   
    
    float wet = gain * fx3mix;
    float dry = gain *(1.0f - fx3mix);

    juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(0), dry, buffer.getNumSamples());
	juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(1), dry, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> audioBlock(buffer); 
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);
    filter.process(context);

	juce::FloatVectorOperations::multiply(buffer.getWritePointer(0), wet, buffer.getNumSamples());
	juce::FloatVectorOperations::multiply(buffer.getWritePointer(1), wet, buffer.getNumSamples());

	juce::FloatVectorOperations::add(buffer.getWritePointer(0), dryBuffer.getReadPointer(0), buffer.getNumSamples());
	juce::FloatVectorOperations::add(buffer.getWritePointer(1), dryBuffer.getReadPointer(1), buffer.getNumSamples());    
    


    
}

void FxseqAudioProcessor::repeater_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer)
{
    int sampleCount=buffer.getNumSamples();
    int sampleRate= getSampleRate();
    int maxBufferSize= int(sampleRate * 60.0 / 1.0 * 4.0); //Nombre d'échantillons lorsque bpm=1
    int bufferSize =int( sampleRate * (60.0 /(float) (bpm) * 4.0));
    int repeatSize=int( sampleRate * (60.0 / (float) (bpm) * repeater_divison));
    

    
    if (isPlaying) {
            if (repeater_buffer.size() != 2) {
                repeater_buffer.resize(2);
                for (int i = 0; i < repeater_buffer.size(); i++) {
                    repeater_buffer[i].clear();
                }
                buffer_pos=0;
            }

            if (repeater_buffer[0].size() >= bufferSize) 
            {
                for (int i = 0; i < repeater_buffer.size(); i++) {
                    repeater_buffer[i].clear();
                }
                buffer_pos=0;
            }

        for (int channel = 0; channel < 2; ++channel) 
        {
            auto* channelData = buffer.getWritePointer(channel);
            if (buffer_pos>repeatSize-sampleCount) {
                buffer_smoothed.setTargetValue(repeater_buffer[channel][0]);
                for (int sample = 0; sample < sampleCount; sample++)
                {
                    repeater_buffer[channel].insert(repeater_buffer[channel].end(),channelData[sample]);                   
                    channelData[sample]=buffer_smoothed.getNextValue();
                }             
            } else {
                for (int sample = 0; sample < sampleCount; sample++)
                {
                    repeater_buffer[channel].insert(repeater_buffer[channel].end(),channelData[sample]);
                    channelData[sample]=repeater_buffer[channel][buffer_pos+sample];
                }
            }
        }
      
        if (buffer_pos < repeatSize ) {
            buffer_pos+=sampleCount;
        } else {
            buffer_pos=0;
        }
        //debug=std::__cxx11::to_string(buffer_pos);


    }
}

/*void FxseqAudioProcessor::repeater_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer)
{
    auto sampleRate = getSampleRate();
    const int bufferSize = repeater_buffer.getNumSamples();
    
    if (repeater_buffer.size() != 2) {
        repeater_buffer.resize(2);
        for (int i = 0; i < repeater_buffer.size(); i++) {
            repeater_buffer[i].clear();
        }
    }
         
    for (auto i = 0; i < 2; ++i) {
        count = 0;
        repeater_buffer.clear();
    }

    for (int channel = 0; channel < 2; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        maxBufferSize = sampleRate * 60.0 / 1.0 * 4.0; 
        bufferSize = sampleRate * (60.0 / bpm * 4.0); 

        for (int i = 0; i < bufferSize; i++) {           
                if (enablePush) {
                    if (repeater_buffer.size() < maxBufferSize) {
                        repeater_buffer.push_back(channelData[i]);
                    }
                    if (repeater_buffer.size() == maxBufferSize) enablePush = false;
                }                
                if (interval != 0) {
                    int divSize = (double)bufferSize / (double)interval;
                    int divCount = count % divSize;
                    const int fadeSize = 100;
                    channelData[i] = buffer[divCount];
                    if (divCount < fadeSize) {
                        channelData[i] *= juce::jmap((double)(divCount), 0.0, (double)(fadeSize - 1), 0.0, 1.0);
                    }
                    else if (divCount > (divSize - fadeSize)) {
                        channelData[i] *= juce::jmap((double)(divCount), (double)(divSize - fadeSize), (double)(divSize - 1), 1.0, 0.0);
                    }
                }
                count++;
        }
    }
}*/
//////////////////////////// FX /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FxseqAudioProcessor::echo_setDelay(float delay)
{
    echoUI->setParamValue("delay",delay);
    echo_time=delay;
}

void FxseqAudioProcessor::echo_setFeedback(float feedback)
{
    echoUI->setParamValue("feedback",feedback);
    echo_feedback=feedback;
}
////////////////////////////// XML /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<unsigned long long int> FxseqAudioProcessor::getPatterns(int seqIndex)
{
    
    std::vector<unsigned long long int> patternsData;
    
    for (int pattern=0;pattern<patternsPerSequencer;pattern++) 
    {  
        unsigned long long int patternData=0;//(n >> k) & 1
        for (int step=0;step<stepsPerPattern;step++)
        {
            for (int bit=0;bit<4;bit++) 
            {
                patternData+=(((unsigned long long int)patterns[seqIndex][pattern][step] >> bit) & 1) * (unsigned long long int)(pow(2,step*4+bit));
            }
        }
        patternsData.push_back(patternData);
    }
    return patternsData;
}

std::vector<unsigned long long int> FxseqAudioProcessor::getSequences()
{
    
    std::vector<unsigned long long int> sequencesData;
    
    for (int sequence=0;sequence<sequenceCount;sequence++) 
    {  
        unsigned long long int sequenceData=0;//(n >> k) & 1
        for (int step=0;step<stepsPerPattern;step++)
        {
            for (int bit=0;bit<4;bit++) 
            {
                sequenceData+=(((unsigned long long int)sequences[sequence][step] >> bit) & 1) * (unsigned long long int)(pow(2,step*4+bit));
            }
        }
        sequencesData.push_back(sequenceData);
    }
    return sequencesData;
}

juce::XmlElement* FxseqAudioProcessor::getAllPatternsXml()
{
    juce::XmlElement* rootElement =new juce::XmlElement ("PATTERNS");
    for (int sequencer=0;sequencer<sequencerCount;sequencer++)
    {
        juce::XmlElement* sequencerElement= new juce::XmlElement (TRANS(fxNamesStr[sequencer]));
        std::vector<unsigned long long int> patternsData=getPatterns(sequencer);
        for (int pat=0;pat<patternsPerSequencer;pat++)
        {
            juce::XmlElement* patXML = new juce::XmlElement (TRANS("PATTERN"+std::__cxx11::to_string(pat)));
            patXML->addTextElement (std::__cxx11::to_string(patternsData[pat]) );
            sequencerElement->addChildElement (patXML);   
        }
        rootElement->addChildElement (sequencerElement); 

         
    }
    return rootElement;
}

juce::XmlElement* FxseqAudioProcessor::getAllSequencesXml()
{
    juce::XmlElement* rootElement =new juce::XmlElement ("SEQUENCES");

    std::vector<unsigned long long int> sequencesData=getSequences();
    for (int sequence=0;sequence<sequenceCount;sequence++)
    {
        juce::XmlElement* seqXML = new juce::XmlElement (TRANS("SEQUENCE"+std::__cxx11::to_string(sequence)));
        seqXML->addTextElement (std::__cxx11::to_string(sequencesData[sequence]) );
        rootElement->addChildElement (seqXML);   
    }
        
    return rootElement;
}

void FxseqAudioProcessor::loadPatternsAndSequencesFromXMLFile(std::string fileName)
{
    if ( not std::filesystem::exists(fileName) )  {

    } else 
    {
        juce::File xmlFile(fileName);
        juce::XmlDocument xmlDoc(xmlFile);

        if (juce::XmlDocument::parse(xmlFile))
        {
            auto rootElement = xmlDoc.getDocumentElement();
            loadPatternsAndSequencesFromXML(rootElement.get());
            pluginParameters.replaceState (juce::ValueTree::fromXml (*rootElement.get()));
        }
        
        initAllPatterns();
    }
}

void FxseqAudioProcessor::loadPatternsAndSequencesFromXML(juce::XmlElement* rootElement)
{   
    //juce::XmlElement* rootElement=&element;
    if (rootElement->hasTagName("PARAMETERS")) 
    {
        int sequencerNum=0; 
        for (auto* e : rootElement->getChildByName("PATTERNS")->getChildIterator())
        {
            int patternNum=0;
            for (auto* f : e->getChildIterator()) 
            {
                std::string paramName = f->getTagName().toStdString();
                std::string paramValue = f->getAllSubText().toStdString();
                //debug+="\n"+ paramName + " : " ;
                for (int i=0;i<stepsPerPattern;i++) 
                {
                    unsigned long long int value=0;
                    for (int j=0;j<4;j++) 
                    {
                        value+=((stoull(paramValue) >> i*4+j) & 1)*(unsigned long long int) (pow(2,j));
                    }
                    patterns[sequencerNum][patternNum][i]=(int)value;
                    //debug+=std::__cxx11::to_string(value) + " " ;
                }  
                patternNum++;
             } 
             sequencerNum++;                            
         }
         int sequenceNum=0;
         for (auto* g : rootElement->getChildByName("SEQUENCES")->getChildIterator())
        {
            std::string paramName = g->getTagName().toStdString();
            std::string paramValue = g->getAllSubText().toStdString();
            //ebug+="\n"+ paramName + " : " + paramValue;
            for (int i=0;i<stepsPerPattern;i++) 
            {
                unsigned long long int value=0;
                for (int j=0;j<4;j++) 
                {
                    value+=((stoull(paramValue) >> i*4+j) & 1)*(unsigned long long int) (pow(2,j));
                }
                sequences[sequenceNum][i]=(int)value;
                //debug+=std::__cxx11::to_string(value) + " " ;
            }  
            sequenceNum++;                            
         }
    } 
       
    
}

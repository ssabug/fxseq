/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "FaustEchoEffect.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    std::string fxNamesStr[4]={"Chopper","Echo","Filter","Crusher"};
    juce::StringArray fxNames=juce::StringArray({"Chopper","Echo","Filter","Crusher"});
 
    params.add (std::make_unique<juce::AudioParameterFloat> ("drywet","Dry/Wet", 0.0f, 1.0f, 1.0f));
    params.add (std::make_unique<juce::AudioParameterFloat> ("outgain","Out Gain",0.0f, 2.0f, 1.0f));

    for (int i = 1; i < 4+1; ++i)
    {
        std::string prefix="seq"+std::__cxx11::to_string(i)+"_";
        params.add (std::make_unique<juce::AudioParameterInt> (prefix+"pattern",prefix+"pattern", 1, 16, 1));
        params.add (std::make_unique<juce::AudioParameterChoice> (prefix+"clockDiv",prefix+"clockDiv",juce::StringArray({ "4","2","1","1/2","1/4","1/8","1/16","1/32" }), 2));
        params.add (std::make_unique<juce::AudioParameterChoice> (prefix+"fx",prefix+"fx",juce::StringArray(fxNames), i));
          
    }
        for (int i = 0; i < 4; ++i)
    {
        std::string prefix=fxNamesStr[i]+"_";
        params.add (std::make_unique<juce::AudioParameterFloat> (prefix+"gain",prefix+"gain", 0.0f, 2.0f, 1.0f));
        params.add (std::make_unique<juce::AudioParameterFloat> (prefix+"dry/wet",prefix+"dry/wet", 0.0f, 1.0f, 1.0f));         
    }

    params.add (std::make_unique<juce::AudioParameterBool> ("sequencerMode","sequencerMode", false));
    params.add (std::make_unique<juce::AudioParameterInt> ("sequenceNumber","sequenceNumber", 1, 16, 1));
    params.add (std::make_unique<juce::AudioParameterInt> ("sequenceLength","sequenceLength", 2, 16, 4));

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
    if (phead != nullptr)
    {
        auto playposinfo=phead->getPosition();
        ppq=*(*playposinfo).getPpqPosition();
        
        for (int i=0;i<sequencerCount;i++) // FOR EACH SEQUENCER
        {   
            sequencerClockMult[i]=(int)pow(2,(pluginParameters.getParameter("seq"+std::__cxx11::to_string(i+1)+"_clockDiv")->getValue()*7));    // get clock mult 
            sequencerPositions[i]=int(std::floor( std::modf(ppq/sequencerClockMult[i],&unused)*(16*resolution))); // compute position on pattern
            if (not (bool)pluginParameters.getParameter("sequencerMode")->getValue()){  // PATTERN MODE          
                selected_pattern[i]=(int)getParameterValue("seq"+std::__cxx11::to_string(i+1)+"_pattern"); //get selected pattern
            } else {                                                                   // SEQUENCE MODE
                int selectedSequence=(int)(pluginParameters.getParameter("sequenceNumber")->getValue()*15); // get selected sequence (SEQUENCE MODE ONLY)                 
                int sequencePosition;
                sequenceLength=(int)getParameterValue("sequenceLength")+2;
                if (ppq >= (float)sequenceLength ) {sequencePosition=int(std::floor(ppq/greatestClockMult)-sequenceLength*std::floor(ppq/(sequenceLength*greatestClockMult)));} else {sequenceLength=std::floor(ppq/greatestClockMult);}         // compute positon on sequence (SEQUENCE MODE ONLY)             
                
                sequencePosition=std::clamp(sequencePosition,0,15); // safety 
                selected_pattern[i]=sequences[selectedSequence][sequencePosition]; // update selected pattern
            }
            lastFxDepths[i]=fxDepths_smoothed[i].getNextValue(); // get smoothed value from previous value
            fxDepths_smoothed[i].setTargetValue(gainPatterns[i][selected_pattern[i]][sequencerPositions[i]]);// get data from pattern and set it for smoothed value target            
        }
        
       
    }
    /*
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {  // Faust echo 
            for (int j = 0; j < buffer.getNumSamples(); j++) {echoInputs[i][j] = *buffer.getWritePointer(i,j); }
    }
    echoDSP->compute(buffer.getNumSamples(),echoInputs,echoOutputs); // Faust echo

    for (int channel = 0; channel < totalNumInputChannels; ++channel) 
    {
        auto* channelData = buffer.getWritePointer (channel);
        //buffer.applyGain(fxDepths_smoothed[0].getNextValue()); // Chopper gain (clicks)
        buffer.applyGainRamp(channel,0,buffer.getNumSamples(),lastFxDepths[0],fxDepths_smoothed[0].getNextValue());// Chopper gain (ramp)
        //for (int i = 0; i < buffer.getNumSamples(); i++){ *buffer.getWritePointer(channel,i) = echoOutputs[channel][i];       } // Faust echo        

    }*/
    
    float fx3mix=getParameterValue("Filter_dry/wet");
    for (int i=0;i<sequencerCount;i++)
    {
        switch (fxPositions[i])
        {
            case 0:
               chopper_process(buffer,fx1drybuffer);//FX1  
               break;
            case 1: 
                echo_process(buffer); // FX2
                break;
            case 2:
                //filter_process(buffer); // FX4
                break;
            case 3:               
                bitcrush_process(buffer); // FX4
                break;
            
        }
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FxseqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FxseqAudioProcessor();
}

void FxseqAudioProcessor::initAllPatterns()
{
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
//////////////////////////// FX CALLBACKS ////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FxseqAudioProcessor::chopper_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer)
{
    float fx1mix=getParameterValue("Chopper_dry/wet");

    for (int channel = 0; channel < 2; ++channel) {
        dryBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());// copy dry input 
        buffer.applyGainRamp(channel,0,buffer.getNumSamples(),lastFxDepths[0],fxDepths_smoothed[0].getNextValue());  // chopper         
    }
    float wet = fx1mix;
    float dry = 1.0f - fx1mix;

    juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(0), dry, buffer.getNumSamples());
	juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(1), dry, buffer.getNumSamples());

	juce::FloatVectorOperations::multiply(buffer.getWritePointer(0), wet, buffer.getNumSamples());
	juce::FloatVectorOperations::multiply(buffer.getWritePointer(1), wet, buffer.getNumSamples());

	juce::FloatVectorOperations::add(buffer.getWritePointer(0), dryBuffer.getReadPointer(0), buffer.getNumSamples());
	juce::FloatVectorOperations::add(buffer.getWritePointer(1), dryBuffer.getReadPointer(1), buffer.getNumSamples());    
}

void FxseqAudioProcessor::echo_process(juce::AudioBuffer<float>& buffer)
{
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
            *buffer.getWritePointer(channel,i) =echoOutputs[channel][i]*echoMix*fx2mix+(1-echoMix*fx2mix) * echoInputs[channel][i]; // echo  
        }
    }
}

void FxseqAudioProcessor::bitcrush_process(juce::AudioBuffer<float>& buffer)
{
    int reduction = int(fxPrograms[1][patterns[3][selected_pattern[3]][int(sequencerPositions[3]/resolution)]][0]);// FX4

    float fx4mix=getParameterValue("Crusher_dry/wet");    
    float crusherMix=std::clamp(fxDepths_smoothed[3].getNextValue(),0.0f,1.0f);//clamp(fxDepths_smoothed[3].getNextValue(),0,1);
    for (int channel = 0; channel < 2; ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); i++){
 
             if (reduction > 1) {if (i%reduction != 0) channelData[i] = crusherMix*fx4mix*channelData[i - i%reduction]+(1-crusherMix*fx4mix)*channelData[i];} // sample rate reduction
        }
    }
}

void FxseqAudioProcessor::filter_process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < 2; ++channel) { //FX3
        juce::dsp::AudioBlock<float> audioBlock(buffer);   // Ladder filter
        juce::dsp::ProcessContextReplacing<float> context(audioBlock);// Ladder filter
        filter.process(context);// Ladder filter
    }   
}

//////////////////////////// FX /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FxseqAudioProcessor::echo_setDelay(float delay)
{
    echoUI->setParamValue("delay",delay);
}

void FxseqAudioProcessor::echo_setFeedback(float feedback)
{
    echoUI->setParamValue("feedback",feedback);
}

float FxseqAudioProcessor::bitcrusher_bitReduction(float sample, float bitReduction)
{
    float maxValue = pow(2, bitReduction) - 1;
    float crushedSample = round(sample * maxValue) / maxValue;
    return crushedSample;
}

float FxseqAudioProcessor::bitcrusher_sampleRateReduction(float sample, int reductionFactor)
{
    if (bitcrusher_count % reductionFactor == 0)
    {
        bitcrusher_count = 0;
        return sample;
    }
    else
    {
        bitcrusher_count++;
        return 0.0f;
    }
}

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/
#include "effects/FaustEffects.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

//==============================================================================
std::vector<std::string> returnFxNames() {return {"Chopper","Echo","Filter","Crusher","Distortion","Repeater","CombFilter","PitchShifter"}; };


juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    std::vector<std::string> fxNamesStr=returnFxNames();
 
    params.add (std::make_unique<juce::AudioParameterFloat> ("drywet","Dry/Wet", 0.0f, 1.0f, 1.0f));
    params.add (std::make_unique<juce::AudioParameterFloat> ("outgain","Out Gain",0.0f, 2.0f, 1.0f));

    params.add (std::make_unique<juce::AudioParameterBool> ("sequencerMode","sequencerMode", false));
    params.add (std::make_unique<juce::AudioParameterInt> ("sequenceNumber","sequenceNumber", 1, 16, 1));
    params.add (std::make_unique<juce::AudioParameterInt> ("sequenceLength","sequenceLength", 2, 16, 4));

    for (int i = 1; i < fxNamesStr.size()+1; ++i)
    {
        std::string prefix=fxNamesStr[i-1]+"_";
        params.add (std::make_unique<juce::AudioParameterInt> (prefix+"pattern",prefix+"pattern", 1, 16, 1));
        params.add (std::make_unique<juce::AudioParameterChoice> (prefix+"clockDiv",prefix+"clockDiv",juce::StringArray({ "4","2","1","1/2","1/4","1/8","1/16","1/32" }), 2));
        params.add (std::make_unique<juce::AudioParameterInt> (prefix+"position",prefix+"position",0, fxNamesStr.size()-1, 1));
        params.add (std::make_unique<juce::AudioParameterFloat> (prefix+"gain",prefix+"gain", 0.0f, 2.0f, 0.1f));
        params.add (std::make_unique<juce::AudioParameterFloat> (prefix+"dry/wet",prefix+"dry/wet", 0.0f, 1.0f, 0.1f));  

        std::vector<EffectParameter> paramList;

        if (fxNamesStr[i-1] == "Repeater")    { DirtyRepeater rpt;    paramList=rpt.params;}
        if (fxNamesStr[i-1] == "Echo")        { Echo ech;        paramList=ech.params;}
        if (fxNamesStr[i-1] == "RingMod")     { RingMod rmd;     paramList=rmd.params;}
        if (fxNamesStr[i-1] == "CombFilter")  { CombFilter cft;  paramList=cft.params;}
        if (fxNamesStr[i-1] == "PitchShifter"){ PitchShifter psh;paramList=psh.params;}
        if (fxNamesStr[i-1] == "Distortion")  { Distortion dst;  paramList=dst.params;}   
        if (fxNamesStr[i-1] == "Chopper")     { Chopper chp;     paramList=chp.params;}
        if (fxNamesStr[i-1] == "Filter")      { Filter flt;      paramList=flt.params;}    
        if (fxNamesStr[i-1] == "Crusher")     { Bitcrusher btc;  paramList=btc.params;}    
        
        for ( auto par : paramList ) {
            if ( par.type == "float") {params.add(std::make_unique<juce::AudioParameterFloat> (prefix+par.id,prefix+par.name, par.min, par.max, par.step));}
            if ( par.type == "int") {params.add(std::make_unique<juce::AudioParameterFloat> (prefix+par.id,prefix+par.name, par.min, par.max, par.step));}
            /*params.add(std::make_unique<juce::AudioParameterInt> (prefx+par.id,prefx+par.name, int(par.min), int(par.max), int(par.step)));*/
        }
        

    }    

    return params;
}
//==============================================================================
FxseqAudioProcessor::FxseqAudioProcessor()
     : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)                     
                                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),  
       pluginParameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    fxNamesStr=returnFxNames();

    initAllPatterns();  
    
    for (int i=0;i<sequencerCount;i++)
    {
        updateParameter(fxNamesStr[i]+"_dry/wet",1.0f);
        updateParameter(fxNamesStr[i]+"_gain",1.0f);
        updateParameter(fxNamesStr[i]+"_position",(float) i);
        sequencerPositions.push_back(0);
        sequencerClockMult.push_back(4);//={4,4,4,4};
        selected_pattern.push_back(0);
        fxPositions.push_back(i);//={0,1,2,3};
        lastFxDepths.push_back(0.0f);
        
    }

    updateParameter("sequencerMode",(float)0);

    // FX Init values
    /*updateParameter("Repeater_count",2.0f);
    updateParameter("Repeater_mSize",800.0f);
    updateParameter("Repeater_size",800.0f);*/
    updateParameter("Repeater_size",1.0f);

    updateParameter("Echo_Time",0.25f);
    updateParameter("Echo_Feedback",0.5f);

    updateParameter("PitchShifter_Window",6000.0f);
    updateParameter("PitchShifter_Xfade",300.0f);
    
    updateParameter("Filter_Frequency",20000.0f);
    updateParameter("Filter_Resonance",0.0f);

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
    masterDryBuffer.setSize(2, samplesPerBlock); 

    // Faust Echo
    echo.prepareToPlay(sampleRate,samplesPerBlock);
    // Faust Ringmod
    ringMod.prepareToPlay(sampleRate,samplesPerBlock);
    // Faust/Dirty Repeater
    repeater.prepareToPlay(sampleRate,samplesPerBlock);
    // Faust PitchShifter
    pitchShifter.prepareToPlay(sampleRate,samplesPerBlock);
    // Chopper
    chopper.prepareToPlay(sampleRate,samplesPerBlock);
    // Filter
    filter.prepareToPlay(sampleRate,samplesPerBlock);
    // Bitcrusher
    bitcrusher.prepareToPlay(sampleRate,samplesPerBlock);
    // Faust combfilter
    combFilter.prepareToPlay(sampleRate,samplesPerBlock);

    // REPEATER
    /*repeater_buffer.clear();
    buffer_pos=0;*/

}

void FxseqAudioProcessor::releaseResources()
{
    
    // Faust Echo
    echo.releaseResources();
    // Faust Ringmod
    ringMod.releaseResources();
    // Faust/Dirty Repeater
    repeater.releaseResources();
    // Faust PitchShifter
    pitchShifter.releaseResources();
    // Chopper
    chopper.releaseResources();
    // Filter
    filter.releaseResources();
    // Bitcrusher
    bitcrusher.releaseResources();
     // Faust combfilter
    combFilter.releaseResources();

    // REPEATER
    /*repeater_buffer.clear();
    buffer_pos=0;*/
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
            if (!(bool)pluginParameters.getParameter("sequencerMode")->getValue()){  // PATTERN MODE          
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
    int numSamples=buffer.getNumSamples();
    /*for (int i=0;i<sequencerCount;i++)
    {
        fxPositions[i]=(int) getParameterValue(fxNamesStr[i] +"_position");
    }*/

    for (int channel = 0; channel < totalNumInputChannels; ++channel) 
    {
        masterDryBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);// copy dry input 
    }
	
    for (int i=0;i<sequencerCount;i++)
    {
        const std::string fxName=fxNamesStr[fxPositions[i]]+"_";
        const std::string gn=fxName+"gain",dw=fxName+"dry/wet";

        switch (fxPositions[i])
        {
            case 0: // CHOPPER
                for ( int par=0;par<chopper.params.size();par++) {chopper.changeParameter(par,getParameterValue(fxName+chopper.params[par].name));}
                chopper.processBlock(buffer,numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue(),lastFxDepths[i]);
                break;
            case 1: // ECHO
                for ( int par=0;par<echo.params.size();par++) {echo.changeParameter(par,getParameterValue(fxName+echo.params[par].name));}
                echo.processBlock(buffer,numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue());
                break;
            case 2: // FILTER
                for ( int par=0;par<filter.params.size();par++) 
                {
                    updateParameter(fxName+filter.params[par].name,fxPrograms[i][patterns[i][selected_pattern[i]][int(sequencerPositions[i]/resolution)]][par]);
                    filter.changeParameter(par,getParameterValue(fxName+filter.params[par].name));                    
                }
                filter.processBlock(buffer, numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue());                             
                break;
            case 3: // BITCRUSHER              
                for ( int par=0;par<bitcrusher.params.size();par++) 
                {
                    updateParameter(fxName+bitcrusher.params[par].name,fxPrograms[i][patterns[i][selected_pattern[i]][int(sequencerPositions[i]/resolution)]][par]);
                    bitcrusher.changeParameter(par,getParameterValue(fxName+bitcrusher.params[par].name));                    
                }
                bitcrusher.processBlock(buffer, numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue());    
                break;
            case 4: // DISTORTION
                for ( int par=0;par<distortion.params.size();par++) {distortion.changeParameter(par,getParameterValue(fxName+distortion.params[par].name));}
                distortion.processBlock(buffer, numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue());
                break;
            case 5: // REPEATER
                if (isPlaying) { 
                    for ( int par=0;par<repeater.params.size();par++) {repeater.changeParameter(par,getParameterValue(fxName+repeater.params[par].name));}
                    //repeater.processBlock(buffer, numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue()); 
                    repeater.processBlock(buffer, numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue(),lastFxDepths[i],bpm,ppq);
                }
                break;
            case 6: // COMBFILTER
                //ringMod_process(buffer);
                /*for ( int par=0;par<ringMod.params.size();par++) {ringMod.changeParameter(par,getParameterValue(fxName+ringMod.params[par].name));}   
                ringMod.changeParameter(2,getParameterValue(gn));
                ringMod.processBlock(buffer, numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue());*/
                for ( int par=0;par<combFilter.params.size();par++) {combFilter.changeParameter(par,getParameterValue(fxName+combFilter.params[par].name));}
                combFilter.processBlock(buffer, numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue());
                break;
            case 7: // PITCHSHIFTER
                for ( int par=0;par<pitchShifter.params.size();par++) {pitchShifter.changeParameter(par,getParameterValue(fxName+pitchShifter.params[par].name));}
                pitchShifter.processBlock(buffer, numSamples, getParameterValue(gn), getParameterValue(dw), fxDepths_smoothed[i].getNextValue());
                break; 
            default:
                break;
        }
    }

    for (int channel = 0; channel < totalNumInputChannels; ++channel) 
    {
        juce::FloatVectorOperations::multiply(masterDryBuffer.getWritePointer(channel), dry, numSamples); // multiply dry buffer by gain * 1-wetvolume
        juce::FloatVectorOperations::multiply(buffer.getWritePointer(channel), wet, numSamples); // multiply wet buffer by gain * wetvolume
	    juce::FloatVectorOperations::add(buffer.getWritePointer(channel), masterDryBuffer.getReadPointer(channel), numSamples);  // add dry to wet   
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
    juce::XmlElement* patternDef=new juce::XmlElement(*(getAllPatternsXml()));
    juce::XmlElement* sequenceDef=new juce::XmlElement(*(getAllSequencesXml()));

    xml->removeChildElement(xml->getChildByName("PATTERNS"),true);
    xml->removeChildElement(xml->getChildByName("SEQUENCES"),true);

    xml->addChildElement(patternDef);
    xml->addChildElement(sequenceDef);

    copyXmlToBinary (*xml, destData);
}

void FxseqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (pluginParameters.state.getType()))
            pluginParameters.replaceState (juce::ValueTree::fromXml (*xmlState));
            loadPatternsAndSequencesFromXML(xmlState.get());
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
                if ( (sequencerIndex == 0) || (sequencerIndex==7) ) { outVector.push_back(enveloppes[value-1][i]);}
                else { outVector.push_back(std::clamp((float)value,0.0f,1.0f)); }
                
            } else {
                outVector.push_back(0.0);
            }
        }
    }        
    return outVector;
}

void FxseqAudioProcessor::updateParameter(std::string paramName,float paramValue)
{
    float rg,min,max;
    min=pluginParameters.getParameter(paramName)->getNormalisableRange().start;
    max= pluginParameters.getParameter(paramName)->getNormalisableRange().end;
    if (min < 0)
    {
        rg=max-min;
        pluginParameters.getParameter(paramName)->setValue((paramValue-min)/rg);
    } else {
        rg=max-min;
        pluginParameters.getParameter(paramName)->setValue(paramValue/rg);
    }
}

float FxseqAudioProcessor::getParameterValue(std::string paramName)
{
    float rg,min,max;
    min=pluginParameters.getParameter(paramName)->getNormalisableRange().start;
    max= pluginParameters.getParameter(paramName)->getNormalisableRange().end;
    if ( min< 0)
    {
        rg=max-min;
        return pluginParameters.getParameter(paramName)->getValue()*rg +min;
    } else {
        rg=max-min;
    }
    return pluginParameters.getParameter(paramName)->getValue()*rg;
}

void FxseqAudioProcessor::updateEffectProgramParameter(int effectIndex,int programIndex,int parameterIndex,float parameterValue)
{
    fxPrograms[effectIndex][programIndex][parameterIndex]=parameterValue;
}

float FxseqAudioProcessor::getEffectProgramParameterValue(int fxIndex,int programIndex,int paramIndex)
{
    //debug="fxIndex " + std::to_string(fxIndex) + " programIndex " + std::to_string(programIndex) + " paramIndex " + std::to_string(paramIndex);
    //return fxPrograms[fxIndex][programIndex][paramIndex];
    return 0.0f;
}

std::vector<std::string> FxseqAudioProcessor::getParameterProperty(int fxIndex,int paramIndex,std::string paramProperty,int programIndex)
{
    //debug+="\n" + paramProperty + std::to_string(fxIndex) + " " + std::to_string(paramIndex);
  // std::ofstream file("/home/pwner/dev/fxseq/log.txt");file << "before shit" ;

   /*std::string fxName=returnFxNames()[fxIndex];//fxNamesStr[0];
    std::vector<EffectParameter> paramList;
    bool hasPrograms=false;
    
    
    if (fxName == "Repeater") 
    {
        //paramList=repeater.params;
        hasPrograms=false;
    }
    
    if (paramProperty == "hasPrograms")  
    {
        return {"0"};
        //return {std::to_string(hasPrograms)}; 
    }
    if (paramProperty == "name")  
    {
        //return {"tarace"};        
        return {paramList[paramIndex].name}; 
    }
    if (paramProperty == "range") 
    {
        std::vector<std::string> result;
        result.push_back(std::to_string(paramList[paramIndex].min));
        result.push_back(std::to_string(paramList[paramIndex].max));
        result.push_back(std::to_string(paramList[paramIndex].step));
        return result; 
    }
    if (paramProperty == "value") 
    {
        if (hasPrograms)
        {
            return {"0"};    
        } else
        {   //return {"100"};    
            return {std::to_string(getParameterValue(fxName + "_" + paramList[paramIndex].name))};
        }
    } */

    return {""};
}
//////////////////////////// FX CALLBACKS ////////////////////////////////////////////////////////////////////////////////////////////////////////////

// REPEATER
/*void FxseqAudioProcessor::repeater_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer)
{
    int sampleCount=buffer.getNumSamples();
    int sampleRate= getSampleRate();
    repeater_divison=fxPrograms[5][patterns[5][selected_pattern[5]][int(sequencerPositions[5]/resolution)]][0];//getParameterValue("Repeater_length");
    int maxBufferSize= int(sampleRate * 60.0 / 1.0 * 4.0); //Nombre d'échantillons lorsque bpm=1
    int bufferSize =int( sampleRate * (60.0 /(float) (bpm) * 4.0));
    int repeatSize=int( sampleRate * (60.0 / (float) (bpm) * 1.0//repeater_divison));
    float mix=fxDepths_smoothed[5].getNextValue()*getParameterValue("Repeater_dry/wet");
    float gain=getParameterValue("Repeater_gain");
    double smooth;
    double lastValue[2];
    int crossFadeStart=1;
    
    if (isPlaying) {

        if (repeater_buffer.size() != 2) {
            repeater_buffer.resize(2);
            for (int i = 0; i < repeater_buffer.size(); i++) {
                repeater_buffer[i].clear();
            }
            buffer_pos=0;
        }

        if (std::modf(ppq/4,&unused) < 0.02f) 
        {
            for (int i = 0; i < repeater_buffer.size(); i++) {
                repeater_buffer[i].clear();
            }
            buffer_pos=0;
        }
        
        double mmix=fxDepths_smoothed[5].getNextValue();

        if (buffer_pos==0//repeatSize-sampleCount*crossFadeStart) {
                lastValue[0]=buffer_smoothed[0].getNextValue();
                lastValue[1]=buffer_smoothed[1].getNextValue();
                //if (buffer_pos<repeatSize-sampleCount*(crossFadeStart-1)) {
                    buffer_smoothed[0].setTargetValue(1.0//repeater_buffer[0][0]);
                    buffer_smoothed[1].setTargetValue(1.0//repeater_buffer[1][0]);
                //}
                //smooth=;
            }
        double crossVolume=buffer_smoothed[0].getNextValue();

        for (int channel = 0; channel < 2; ++channel) 
        {
            auto* channelData = buffer.getWritePointer(channel);
            auto* repeatData=  fx2drybuffer.getWritePointer(channel);
            auto* startData=  fx4dryBuffer.getWritePointer(channel);

            
            
            for (int sample = 0; sample < sampleCount; sample++)
            {
                //if (buffer_pos>repeatSize-sampleCount*4) {                   
                //    repeater_buffer[channel].insert(repeater_buffer[channel].end(),buffer_smoothed[channel].getNextValue());                                                  
               // } else {
                repeater_buffer[channel].insert(repeater_buffer[channel].end(),channelData[sample]);                                    
              //}
                startData[sample]=repeater_buffer[channel][sample];

                //channelData[sample]=gain * mix * interpolated[channel].getNextValue()+(1.0f-mix)*channelData[sample] ;
               repeatData[sample]=repeater_buffer[channel][buffer_pos+sample];

            }

            if (buffer_pos==0//>repeatSize-sampleCount*crossFadeStart) {
                fx2drybuffer.applyGainRamp(channel,0,sampleCount,1.0,1-crossVolume); // crossfade current buffer and start buffer
                fx4dryBuffer.applyGainRamp(channel,0,sampleCount,0,crossVolume);
                juce::FloatVectorOperations::add(fx2drybuffer.getWritePointer(channel), fx4dryBuffer.getReadPointer(channel), sampleCount);
            }

            fx2drybuffer.applyGainRamp(channel,0,sampleCount,lastFxDepths[5],mmix);
            buffer.applyGainRamp(channel,0,sampleCount,1.0f-lastFxDepths[5],1.0f-mmix);
            juce::FloatVectorOperations::add(buffer.getWritePointer(channel), fx2drybuffer.getReadPointer(channel), sampleCount);
        }
      
        if (buffer_pos < repeatSize ) {
            buffer_pos+=sampleCount;
        } else {
            buffer_pos=0;
        }
        //debug=std::to_string(buffer_pos);


    }
}*/

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


// COMBFILTER
/*void FxseqAudioProcessor::combFilter_process(juce::AudioBuffer<float>& buffer)
{
    float gain=getParameterValue("CombFilter_gain");
    float fx2mix=getParameterValue("CombFilter_dry/wet");    
    float echoMix=fxDepths_smoothed[7].getNextValue();

    for (int channel = 0; channel < 1; ++channel) {
        for (int i = 0; i < buffer.getNumSamples(); i++) { 
                combFilterInputs[channel][i] = *buffer.getWritePointer(channel,i); //Echo
            }
    }
    combFilterDSP->compute(buffer.getNumSamples(),combFilterInputs,combFilterOutputs);
    for (int channel = 0; channel < 2; ++channel) {
        for (int i = 0; i < buffer.getNumSamples(); i++){
            *buffer.getWritePointer(channel,i) =gain*combFilterOutputs[0][i]*echoMix*fx2mix+(1-echoMix*fx2mix) * combFilterInputs[0][i]; // echo  
        }
    }
}*/


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
            juce::XmlElement* patXML = new juce::XmlElement (TRANS("PATTERN"+std::to_string(pat)));
            patXML->addTextElement (std::to_string(patternsData[pat]) );
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
        juce::XmlElement* seqXML = new juce::XmlElement (TRANS("SEQUENCE"+std::to_string(sequence)));
        seqXML->addTextElement (std::to_string(sequencesData[sequence]) );
        rootElement->addChildElement (seqXML);   
    }
        
    return rootElement;
}

void FxseqAudioProcessor::loadPatternsAndSequencesFromXMLFile(std::string fileName)
{
    if ( !std::filesystem::exists(fileName) )  {

    } else 
    {
        juce::File xmlFile(fileName);
        juce::XmlDocument xmlDoc(xmlFile);

        if (juce::XmlDocument::parse(xmlFile))
        {
            auto rootElement = xmlDoc.getDocumentElement();
            loadPatternsAndSequencesFromXML(rootElement.get());
            pluginParameters.replaceState (juce::ValueTree::fromXml (*rootElement.get()));
            initAllPatterns();
        }
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
                    //debug+=std::to_string(value) + " " ;
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
                //debug+=std::to_string(value) + " " ;
            }  
            sequenceNum++;                            
         }
    } 
       
    
}

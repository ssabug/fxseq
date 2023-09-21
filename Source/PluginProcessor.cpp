/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FxseqAudioProcessor::FxseqAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void FxseqAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
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
                     
            sequencerPositions[i]=int(std::floor( std::modf(ppq/sequencerClockMult[i],&unused)*(16*resolution))); // compute position on pattern
            if (not sequenceMode)           
            {
                     
            } else {
              int selectedSequence=0;         // get selected sequence (SEQUENCE MODE ONLY)   
              int sequencePosition=0;         // compute positon on sequence (SEQUENCE MODE ONLY)             
              selected_pattern[i]=sequences[selectedSequence][sequencePosition];
            }
            //lastFxDepths[i]=fxDepths_smoothed[i].getNextValue(); // get smoothed value from previous value
            //fxDepths_smoothed[i].setTargetValue(gainPatterns[i][selected_pattern[i]][sequencerPositions[i]]);// get data from pattern and set it for smoothed value target );           
        }
        
       
    }
   
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
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
        for (int j=0;j<patternsPerSequencer;j++)
        {
            gainPatterns[i][j]=generateGainPattern(i,j);
        }
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

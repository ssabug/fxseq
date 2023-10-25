/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "effects/FaustRepeater.h"
#include "effects/FaustEcho.h"
#include "effects/FaustRingMod.h"
#include "effects/FaustCombFilter.h"
#include "effects/FaustPitchShifter.h"
#include "effects/Distortion.h"
#include "effects/Chopper.h"
#include "effects/Filter.h"
#include "effects/Bitcrusher.h"
#include "effects/DirtyRepeater.h"

//====FAUST DSP CLASSES=========================
class MapUI;
class dsp;

//==============================================================================
/**
*/
class FxseqAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    FxseqAudioProcessor();
    ~FxseqAudioProcessor() override;
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    //==============================================================================
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //===============================================================================
    std::vector<unsigned long long int> getPatterns(int seqIndex);
    juce::XmlElement* getAllPatternsXml();
    std::vector<unsigned long long int> getSequences();
    juce::XmlElement* getAllSequencesXml();
    juce::XmlElement* getProgramParametersXml();
    void loadPatternsAndSequencesFromXML(juce::XmlElement* rootElement);
    void loadPatternsAndSequencesFromXMLFile(std::string fileName);
    void loadProgramsFromXML(juce::XmlElement* rootElement);
    //==============================================================================
    std::vector<std::string> split(std::string s, std::string delimiter);
    //========================================================================================
    void initAllPatterns();
    std::vector<float> generateGainPattern(int sequencerIndex,int patternIndex);
    void updateGainPattern(int sequencerIndex,int patternIndex);
    void updateParameter(std::string paramName,float paramValue);
    float getParameterValue(std::string paramName);
    std::vector<std::string> getParameterProperty(int fxIndex,int paramIndex,std::string paramProperty,int programIndex);
    void updateEffectProgramParameter(int effectIndex,int programIndex,int parameterIndex,float parameterValue); 
    float getEffectProgramParameterValue(int fxIndex,int programIndex,int paramIndex);

    double ppq;
    double bpm=1.0f;
    bool isPlaying;
    float unused;
    int resolution=8;
    const int sequencerCount=8;
    const int sequenceCount=16;
    int patternsPerSequencer=16;
    int stepsPerPattern=16;
    std::vector<int> sequencerPositions;
    std::vector<int> sequencerClockMult;//={4,4,4,4};
    std::vector<int> selected_pattern;
    std::vector<int> fxPositions;//={0,1,2,3};
    std::vector<float> lastFxDepths;
    std::vector<std::string> fxNamesStr;//={"Chopper","Echo","Filter","Crusher","Distortion","Repeater","RingMod","PitchShifter"};
    int greatestClockMult=4; 
    int sequenceLength=4;

    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> fxDepths_smoothed[8];
    juce::AudioBuffer<float> masterDryBuffer; 
      
    std::string debug;  

    std::vector<std::vector<std::vector<float>>> gainPatterns;
    std::vector<float> testGainPatterns;
    std::vector<std::vector<std::vector<int>>> patterns={   {   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//chopper
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                            {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//echo
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                            {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},//filter
                                                                {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4},{1,1,2,2,1,1,3,3,1,1,2,2,1,1,4,4},{1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}     
                                                              },
                                                            {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},//crusher
                                                                {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                             {  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//dist
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}         
                                                              },
                                                             {  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,0,0,1,0,1,0,0,0,1,1,1,1},{1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},//repeat
                                                                {0,0,1,1,0,0,1,1,1,1,1,1,0,0,0,0},{0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,1},{0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0},{1,1,1,1,0,0,0,0,1,0,0,0,0,0,0,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}       
                                                              },
                                                             {  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//ringmod
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}         
                                                              },
                                                             {  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},//pitchsh
                                                                {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4},{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },                                                        
                                                               };

    std::vector<std::vector<float>> enveloppes={  {1.00,1.00,1.00,1.00,1.00,1.00,1.00,1.00},   // FLAT
                                                  {1.00,0.87,0.75,0.50,0.38,0.25,0.12,0.00},   // SAW  
                                                  {0.00,0.25,0.50,0.75,0.75,0.50,0.25,0.00},   // TRI
                                                  {0.00,0.12,0.25,0.38,0.50,0.75,0.87,1.00},   // RAMP    
                                                  {0.50,0.75,1.00,0.75,0.50,0.25,0.00,0.50},   // SINE
                                                  {1.00,0.75,0.25,0.00,0.25,0.50,0.78,1.00}    // INVERTED TRI                                   
                                                };  

    std::vector<std::vector<int>> sequences={           {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                                        {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                                        {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                                                        {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},{15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
                                                    };

    std::vector<std::vector<std::vector<float>>> fxPrograms={ {},                    // CHOPPER
                                                              {},                    // ECHO
                                                              { {20000.0f  ,0.0f,   1.0f},  // FILTER     : frequency, resonance,drive
                                                                {50.0f,     0.5f,   1.0f},
                                                                {100.0f,    0.5f,   1.0f},
                                                                {150.0f,    0.5f,   1.0f},
                                                                {200.0f,    0.5f,   1.0f},
                                                                {250.0f,    0.5f,   1.0f},
                                                                {300.0f,    0.5f,   1.0f}
                                                               },
                                                              { {00.0f,    32.00f},  // bitcrusher : sampleReduction, bitdepth
                                                                {06.0f,    32.00f},
                                                                {08.0f,    16.00f},
                                                                {10.0f,    8.00f},
                                                                {12.0f,    2.00f}
                                                              },
                                                              {},                    // distortion
                                                              { {1.0},                     //repeater :length
                                                                {1.0},
                                                                {2.0},
                                                                {3.0},
                                                                {4.0}
                                                               },
                                                                {},                 // ringmod
                                                                {}                  // pitchshifter
                                                            }; 
    juce::AudioProcessorValueTreeState pluginParameters; 
  
    //====Effects======================================================================
    DirtyRepeater repeater;
    //Repeater repeater;
    Echo echo;
    RingMod ringMod;
    PitchShifter pitchShifter;
    Distortion distortion;
    Chopper chopper;
    Filter filter;
    Bitcrusher bitcrusher;
    CombFilter combFilter;
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxseqAudioProcessor)
};

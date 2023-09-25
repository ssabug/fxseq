/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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
    //==============================================================================
    void initAllPatterns();
    std::vector<float> generateGainPattern(int sequencerIndex,int patternIndex);
    void updateGainPattern(int sequencerIndex,int patternIndex);
    void updateParameter(std::string paramName,float paramValue);
    float getParameterValue(std::string paramName);
    void updateEffectProgramParameter(int effectIndex,int programIndex,int parameterIndex,float parameterValue); 

    double ppq;
    float unused;
    int resolution=8;
    const int sequencerCount=4;
    int patternsPerSequencer=16;
    int stepsPerPattern=16;
    int sequencerPositions[4];
    int sequencerClockMult[4]={4,4,4,4};
    int selected_pattern[4];
    int greatestClockMult=4; 
    int sequenceLength=4;
    std::vector<int> fxPositions={0,1,2,3};  

    std::vector<std::vector<std::vector<float>>> gainPatterns;
    std::vector<float> testGainPatterns;
    std::vector<std::vector<std::vector<int>>> patterns={   {   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                            {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                            {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                            {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0},{1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
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

    std::vector<std::vector<std::vector<float>>> fxPrograms={ { {20000.0f   ,0.0f},  // FILTER
                                                                {50.0f      ,0.5f},
                                                                {100.0f     ,0.5f},
                                                                {150.0f     ,0.5f},
                                                                {200.0f     ,0.5f},
                                                                {250.0f     ,0.5f},
                                                                {300.0f     ,0.5f}
                                                               },
                                                              { {00.00f     ,0.0f},  // bitcrusher
                                                                {06.0f      ,0.5f},
                                                                {08.0f      ,0.5f},
                                                                {10.0f      ,0.5f},
                                                                {12.0f      ,0.5f}
                                                              }
                                                            };
    
    juce::AudioBuffer<float> fx1drybuffer,fx2drybuffer,fx3drybuffer,fx4dryBuffer; 
    juce::dsp::DryWetMixer<float> fx1dryWetMixer,fx2dryWetMixer,fx3dryWetMixer,fx4dryWetMixer;
    float lastFxDepths[4];
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> fxDepths_smoothed[4];
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear> filterFreq_smoothed;
    float filterFreq_last;
    juce::AudioProcessorValueTreeState pluginParameters; 

    //=====FAUST=ECHO=================================================================   
    void echo_setDelay(float delay);
    void echo_setFeedback(float feedback);
    //=====FILTER=================================================================
    juce::dsp::LadderFilter<float> filter;
private:
    //=====CHOPPER===================================================================
    void chopper_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer);
    //=====FAUST=ECHO=================================================================
    void echo_process(juce::AudioBuffer<float>& buffer);
    MapUI* echoUI;
    dsp* echoDSP;
    float **echoInputs;
    float **echoOutputs;
    //====BITCRUSHER===================================================================
    void bitcrush_process(juce::AudioBuffer<float>& buffer);
    float bitcrusher_bitReduction(float sample, float bitReduction);
    float bitcrusher_sampleRateReduction(float sample, int reductionFactor);
    int bitcrusher_count ;

    void filter_process(juce::AudioBuffer<float>& buffer);
    //juce::AudioProcessorValueTreeState pluginParameters;   
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxseqAudioProcessor)
};

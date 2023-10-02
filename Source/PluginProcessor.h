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
class rmdsp;

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
    std::vector<unsigned long long int> getPatterns(int seqIndex);
    juce::XmlElement* getAllPatternsXml();
    std::vector<unsigned long long int> getSequences();
    juce::XmlElement* getAllSequencesXml();
    void loadPatternsAndSequencesFromXML(juce::XmlElement* rootElement);
    void loadPatternsAndSequencesFromXMLFile(std::string fileName);
    //==============================================================================
    void initAllPatterns();
    std::vector<float> generateGainPattern(int sequencerIndex,int patternIndex);
    void updateGainPattern(int sequencerIndex,int patternIndex);
    void updateParameter(std::string paramName,float paramValue);
    float getParameterValue(std::string paramName);
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
    const std::vector<std::string> fxNamesStr={"Chopper","Echo","Filter","Crusher","Distortion","Repeater","RingMod","PitchShifter"};
    int greatestClockMult=4; 
    int sequenceLength=4;

    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> fxDepths_smoothed[8];
    juce::AudioBuffer<float> masterDryBuffer,fx1drybuffer,fx2drybuffer,fx3drybuffer,fx4dryBuffer; 
    juce::dsp::DryWetMixer<float> fx1dryWetMixer,fx2dryWetMixer,fx3dryWetMixer,fx4dryWetMixer;
    
   
    std::string debug;  

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
                                                            {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
                                                                {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}     
                                                              },
                                                            {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
                                                                {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                             {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
                                                                {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                             {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
                                                                {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                             {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
                                                                {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                                                {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0}           
                                                              },
                                                             {   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
                                                                {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},{1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
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
                                                              { {00.0f,    10.00f},  // bitcrusher : sampleReduction, bitdepth
                                                                {06.0f,    08.00f},
                                                                {08.0f,    32.00f},
                                                                {10.0f,    32.00f},
                                                                {12.0f,    32.00f}
                                                              },
                                                              {},                    // distortion
                                                              { {1.0},                     //repeater :length
                                                                {1.0},
                                                                {2.0},
                                                                {3.0},
                                                                {4.0}
                                                               }
                                                            }; 
    juce::AudioProcessorValueTreeState pluginParameters; 
    //=====FAUST=ECHO=================================================================   
    void echo_setDelay(float delay);
    void echo_setFeedback(float feedback);
    float echo_time=0.25f,echo_feedback=0.5f;
    //=====FAUST=RINGMOD================================================================
    void ringMod_setDepth(float depth);
    void ringMod_setGain(float gain);
    void ringMod_setFreq(float freq);
    float ringMod_freq=60.0f,ringMod_depth=0.5f,ringMod_gain=1.0f;
    //=====FAUST=COMBFILTER================================================================
    void combFilter_setFrequency(float frequency);
    void combFilter_setFeedback(float feedback);
    float combFilter_frequency=1000.0f,combFilter_feedback=0.5f;
    //=====FAUST=PITCHSHIFTER================================================================
    void pitchShifter_setPitch(float pitch);
    void pitchShifter_setXfade(float xfade);
    void pitchShifter_setWindow(float window);
    float pitchShifter_frequency=0.0f,pitchShifter_xfade=0.5f,pitchShifter_window=1.0f;
    //=====FILTER=================================================================
    juce::dsp::LadderFilter<float> filter;
    //====REPEATER======================================================================
    float repeater_divison=1.0;
private:
    //=====CHOPPER===================================================================
    void chopper_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer);
    //=====FAUST=ECHO=================================================================
    void echo_process(juce::AudioBuffer<float>& buffer);
    MapUI* echoUI;
    dsp* echoDSP;
    float **echoInputs;
    float **echoOutputs;
    //=====FAUST=RINGMOD=================================================================
    void ringMod_process(juce::AudioBuffer<float>& buffer);
    MapUI* ringModUI;
    dsp* ringModDSP;
    float **ringModInputs;
    float **ringModOutputs;
    //=====FAUST=COMBFILTER=================================================================
    void combFilter_process(juce::AudioBuffer<float>& buffer);
    MapUI* combFilterUI;
    dsp* combFilterDSP;
    float **combFilterInputs;
    float **combFilterOutputs;
    //=====FAUST=PITCHSHIFTER=================================================================
    void pitchShifter_process(juce::AudioBuffer<float>& buffer);
    MapUI* pitchShifterUI;
    dsp* pitchShifterDSP;
    float **pitchShifterInputs;
    float **pitchShifterOutputs;
     //====FILTER===================================================================
    void filter_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer);
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear> filterFreq_smoothed;
    float lastFilterFrequency=20000.0f;
    float lastFilterResonance=0.0f;
    float lastFilterDrive=0.0f;
    //====BITCRUSHER===================================================================
    void bitcrush_process(juce::AudioBuffer<float>& buffer);  
    //====DISTORTION===================================================================
    void distortion_route(juce::AudioBuffer<float>& buffer);
    void distortion_process(juce::AudioBuffer<float>& buffer);
    void hardclip_process(juce::AudioBuffer<float>& buffer);
    //====REPEATER======================================================================
    void repeater_process(juce::AudioBuffer<float>& buffer,juce::AudioBuffer<float>& dryBuffer);
    std::vector<std::vector<float>> repeater_buffer;
    int buffer_pos=0;

    int count = 0; 
    int maxBufferSize;
    int bufferSize;
    bool enablePush = false;
    int interval = 0;
    int preserveInterval = 0;
    //repeater_maxBufferSize;
    //repeater_bufferSize;
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear> buffer_smoothed[2];
    juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear> interpolated[2];
    //juce::AudioProcessorValueTreeState pluginParameters;   
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxseqAudioProcessor)
};

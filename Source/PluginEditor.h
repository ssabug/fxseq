/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <filesystem>
#include <fstream>

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SequencerComponent.h"
#include "SequenceSequencerComponent.h"
#include "EffectComponent.h"
#include "OptionsComponent.h"
#include "OutputComponent.h"



//==============================================================================
/**
*/
class FxseqAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                   public juce::Timer
{
    public:
        FxseqAudioProcessorEditor (FxseqAudioProcessor&);
        ~FxseqAudioProcessorEditor() override;

        //==============================================================================
        void paint (juce::Graphics&) override;
        void resized() override;

        void updateSeqPattern(int sequencerIndex,int patternIndex);
        void updateProcessorPattern(int sequencerIndex,int patternIndex);
        void updateSelectedProcessorPattern(int sequencerIndex,int patternIndex);
        void updateSelectedProcessorClock(int sequencerIndex,int clockIndex);
        void updateSelectedProcessorEffect(int sequencerIndex,int effectIndex);
    
        std::string skinPath="/home/pwner/dev/fxseq/Ressources/skins/default/";
        std::string imagePath=skinPath+"images/";
    private:
        void timerCallback();
        //UTILS
        std::vector<std::string> split(std::string s, std::string delimiter);
        // XML //
        std::string readXMLParam(std::string xmlFilePath,std::string paramPath);
        std::vector<std::string> readXMLVectorParam(std::string xmlFilePath,std::string paramPath);
        std::vector<std::vector<int>> readXMLVector2Param(std::string xmlFilePath,std::string paramPath);

        juce::Component scene;
        juce::TextEditor debugLog;
        SequencerComponent sequencers[4]={SequencerComponent(0,this,"imagebutton1","combo1"),
                                          SequencerComponent(1,this,"imagebutton2","combo2"),
                                          SequencerComponent(2,this,"imagebutton3","combo3"),
                                          SequencerComponent(3,this,"imagebutton4","combo4")
                                         };

        SequenceSequencerComponent sequenceSeq={SequenceSequencerComponent(this,"combo3","slider3","imagebutton3")};

        EffectComponent effects[4]=      {EffectComponent(0,this,"CHOPPER","combo1","slider1","programbutton1"),
                                          EffectComponent(1,this,"FILTER","combo2","slider2","programbutton2"),
                                          EffectComponent(2,this,"ECHO","combo3","slider3","programbutton3"),
                                          EffectComponent(3,this,"CRUSHER","combo4","slider4","programbutton4")
                                         };

        OptionsComponent options= OptionsComponent(this,"combo1","slider1","textbutton1");
    
        OutputComponent output= OutputComponent(this,"combo1","slider1","imagebutton1");
        
        FxseqAudioProcessor& audioProcessor;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxseqAudioProcessorEditor)
};

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SequencerComponent.h"
#include "EffectComponent.h"
#include "OptionsComponent.h"
#include <filesystem>
#include <fstream>

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

        EffectComponent effects[4]=      {EffectComponent(0,"CHOPPER","combo1","slider1"),
                                          EffectComponent(1,"FILTER","combo2","slider2"),
                                          EffectComponent(2,"ECHO","combo3","slider3"),
                                          EffectComponent(3,"CRUHSER","combo4","slider4")
                                         };
        
        FxseqAudioProcessor& audioProcessor;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxseqAudioProcessorEditor)
};

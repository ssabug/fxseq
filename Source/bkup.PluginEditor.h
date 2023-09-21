/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SequencerComponent   : public juce::Component
{
    public:
        SequencerComponent(/*juce::AudioProcessorEditor**/);
        void updatePosition(int position);
        void updatePattern(int patternData[]);
        void resized() override;

        const int stepCount=16;
        int stepMaxValue=1;
        int pattern[16];
        juce::AudioProcessorEditor *APE;

        juce::ImageButton seqStep[16];      
    private:
        void seqStepClick(int stepIndex); 
     
        
        std::string skinPath="/home/pwner/dev/fxseq/Ressources/skins/default/"; 
        std::string imagePath=skinPath+ "images/";
        juce::Image img_stepSeqOff=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_off.png"));
        juce::Image img_stepSeqOn=juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_on.png"));
     
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerComponent)
};

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
    private:
        void timerCallback();
        juce::Component scene;
        juce::TextEditor debugLog;
        SequencerComponent sequencers[2];
        int sequencersCount=2;
        
        FxseqAudioProcessor& audioProcessor;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxseqAudioProcessorEditor)
};

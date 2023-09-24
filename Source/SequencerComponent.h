/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//==============================================================================
/**
*/
class FxseqAudioProcessorEditor;

class SequencerComponent   : public juce::Component
{
    public:
        SequencerComponent(int Index,FxseqAudioProcessorEditor *ape,std::string StepSeqTemplate,std::string ComboTemplate);
        void resized() override;
        //CALLBACKS
        void skinChange();
        void updatePosition(int position);
        void updatePattern(int patternData[]);        
        void changeSelectedPattern();
        void changeClockMult();
        void changeEffect();
        int getSelectedPattern();
        void remoteChangeSelectedPattern(int patternToSelect);
        void changeSequencerPosition(bool up);

        FxseqAudioProcessorEditor *APE;

        std::string skinPath="/home/pwner/dev/fxseq/Ressources/skins/default/"; 
        std::string imagePath=skinPath+ "images/";

        int index;
        int position;
        const int stepCount=16;
        int stepMaxValue=1;
        std::vector<int> pattern;
        int clockMult=4;

        std::string stepSeqTemplate="imagebutton1";
        std::vector<juce::Image> stepSeqImages={juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_off.png")),
                                                juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_on.png"))
                                                };
        std::vector<std::vector<int>> stepSeqColors={{(int)0x1000280,(int)0xFF00FF00},{(int)0x1000281,(int)0xFF007F00}};

        std::string comboTemplate="combo1";
        std::vector<std::vector<int>> comboColors={{(int)0x1000b00,(int)0xFF000000},{(int)0x1000c00,(int)0xFF00FF00},{(int)0x1000e00,(int)0xFF00FF00},{(int)0x1000a00,(int)0xFF00AF00}};
          
    private:
        void seqStepClick(int stepIndex); 
        
        juce::ImageButton seqStep[16];
        juce::ComboBox  patternSelect,clockMultSelect,effectTypeSelect;
        juce::Label     seqStepMkr[4],patternSelectLabel,clockMultSelectLabel;
        juce::TextButton seqMoveUpButton,seqMoveDownButton;  
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequencerComponent)
};


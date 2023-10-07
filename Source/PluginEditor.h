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
#include "ui/SequencerComponent.h"
#include "ui/SequenceSequencerComponent.h"
#include "ui/EffectComponent.h"
#include "ui/OptionsComponent.h"
#include "ui/OutputComponent.h"



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
        void updateSelectedProcessorEffect(int sequencerIndex,int effectIndex);
        std::vector<int> getSequence(int seqIndex);
        void updateSequence(int seqIndex, std::vector<int> sequence);
        void changeFxPosition(int seqIndex,int newPosition);
        int getSequencerPosition(int seqIndex);
        void refreshSequencerPositions();
        int getSequencerCount();
        void updateFxParam(int fxIndex,int programIndex, float paramIndex,float paramValue);
        std::vector<std::string> getFxParamProperty(int fxIndex, int paramIndex, int programIndex,std::string paramProperty);
        void patternUtils(std::string action,int seqIndex);
        void updateMaster(std::string parameterName,float value);
        float getMasterParam(std::string parameterName);
        //=======FILES=&=XML=================================================
        std::vector<std::string> getFileSeparators();
        std::string getPath(std::string path);
        void initDirectories();
        std::vector<std::string> get_directories(const std::string& s);
        std::vector<std::string> getPresetList();
        std::vector<std::string> getSkinList();
        std::string removeForbiddenCharacters(std::string text);
        bool presetExists(std::string presetText);
        void saveXMLPreset(std::string presetName);
        void loadPreset(std::string presetName);

        int greatestClockMult=4;
        std::vector<std::string> fxNamesStr={"Chopper","Echo","Filter","Crusher","Distortion","Repeater","CombFilter","PitchShifter"};
        std::vector<int> clipboard;

        std::string currentSkin="default",currentPreset="default",rootPath="/home/pwner/dev/fxseq/Ressources/";
        std::string debugtext;
    private:
        void timerCallback();
        //UTILS
        std::vector<std::string> split(std::string s, std::string delimiter);
        int greatest(int n1,int n2,int n3,int n4);
        int greatest(std::vector<int> seqClockDivs);
        int lowest(int n1,int n2,int n3,int n4);
        // XML //
        std::string readXMLParam(std::string xmlFilePath,std::string paramPath);
        std::vector<std::string> readXMLVectorParam(std::string xmlFilePath,std::string paramPath);
        std::vector<std::vector<int>> readXMLVector2Param(std::string xmlFilePath,std::string paramPath);

        juce::Component scene;

        juce::TextEditor debugLog;
        
        SequencerComponent sequencers[8]={SequencerComponent(0,this,"imagebutton1","combo1"),
                                          SequencerComponent(1,this,"imagebutton2","combo2"),
                                          SequencerComponent(2,this,"imagebutton3","combo3"),
                                          SequencerComponent(3,this,"imagebutton4","combo4"),
                                          SequencerComponent(4,this,"imagebutton5","combo5"),
                                          SequencerComponent(5,this,"imagebutton6","combo6"),
                                          SequencerComponent(6,this,"imagebutton7","combo7"),
                                          SequencerComponent(7,this,"imagebutton8","combo8")
                                         };

        SequenceSequencerComponent sequenceSeq={SequenceSequencerComponent(this,"combo1","slider1","imagebutton1")};

        EffectComponent effects[8]=      {EffectComponent(0,this,"Chopper","combo1","slider1","programbutton1"),
                                          EffectComponent(1,this,"Echo","combo2","slider2","programbutton2"),
                                          EffectComponent(2,this,"Filter","combo3","slider3","programbutton3"),
                                          EffectComponent(3,this,"Crusher","combo4","slider4","programbutton4"),
                                          EffectComponent(4,this,"Distortion","combo5","slider5","programbutton5"),
                                          EffectComponent(5,this,"Repeater","combo6","slider6","programbutton6"),
                                          EffectComponent(6,this,"CombFilter","combo7","slider7","programbutton7"),
                                          EffectComponent(7,this,"PitchShifter","combo8","slider8","programbutton8"),
                                         };

        OptionsComponent options= OptionsComponent(this,"combo1","slider1","textbutton1");
    
        OutputComponent output= OutputComponent(this,"combo1","slider1","imagebutton1");
        
        juce::ImageButton logo;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outMixAttachement,outGainAttachement;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fxOutMixAttachement[8],fxOutGainAttachement[8],sequenceLengthAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> patternSelectAttachement[8],clockMultSelectAttachment[8],sequencerModeAttachment,sequenceSelectedAttachment;
        
        FxseqAudioProcessor& audioProcessor;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FxseqAudioProcessorEditor)
};

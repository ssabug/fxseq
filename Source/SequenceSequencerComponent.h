#pragma once

#include <JuceHeader.h>

class FxseqAudioProcessorEditor;

class SequenceSequencerComponent   : public juce::Component
{
    public:
        SequenceSequencerComponent(FxseqAudioProcessorEditor *ape,std::string ComboTemplate,std::string SliderTemplate,std::string ImageButtonTemplate);
        void resized() override;
        //CALLBACKS
        void skinChange();
        void changeSelectedSequence();
        void sequenceChanged(int step);
        void updateSequence(int sequenceIndex);
        void changeSequenceLength();

        FxseqAudioProcessorEditor *APE;

        int selectedSequence;

        std::string comboTemplate="combo3",sliderTemplate="slider3",imageButtonTemplate="imagebutton3";
        std::vector<std::vector<int>> comboColors={{(int)0x1000b00,(int)0xFF000000},{(int)0x1000c00,(int)0xFF7F00FF},{(int)0x1000e00,(int)0xFF7F00FF},{(int)0x1000a00,(int)0xFF7F00FF}};
        std::vector<std::vector<int>> sliderColors={{(int)0x1001300,(int)0xFF00FF00},{(int)0x1001312,(int)0xFF000000},{(int)0x1001311,(int)0xFF00FF00},{(int)0x1001400,(int)0xFF00AF00},{(int)0x1001700,(int)0xFF000000}};
        std::vector<std::vector<int>> imageButtonColors={{(int)0x1000280,(int)0xFF00FF00},{(int)0x1000281,(int)0xFF007F00}};

    private:
        void initSlider2(juce::Slider& slider,float min,float max,float def);

        juce::ComboBox sequenceStep[16],sequenceSelect;
        juce::TextButton autoScroll;
        juce::Label sequenceSelectLabel,autoScrollLabel,sequenceLengthSliderLabel;
        juce::Slider sequenceLengthSlider;
        
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SequenceSequencerComponent)
};   

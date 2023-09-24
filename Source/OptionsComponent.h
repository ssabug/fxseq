#pragma once

#include <JuceHeader.h>

class FxseqAudioProcessorEditor;

class OptionsComponent   : public juce::Component
{
    public:
        OptionsComponent(FxseqAudioProcessorEditor *ape,std::string ComboTemplate,std::string SliderTemplate,std::string ImageButtonTemplate);
        void resized() override;
        //CALLBACKS
        void skinChange();
        void changeSelectedPreset();
        void savePreset();
        void changeSequencerMode();
        void switchScroll();
        void debugFunction();

        FxseqAudioProcessorEditor *APE;

        bool scroll=true;
        bool sequenceMode=false;

        std::string comboTemplate="combo1",sliderTemplate="slider1",imageButtonTemplate="textbutton1";
        std::vector<std::vector<int>> comboColors={{(int)0x1000b00,(int)0xFF000000},{(int)0x1000c00,(int)0xFF00FF00},{(int)0x1000e00,(int)0xFF00FF00},{(int)0x1000a00,(int)0xFF00AF00}};
        std::vector<std::vector<int>> sliderColors={{(int)0x1001300,(int)0xFF00FF00},{(int)0x1001312,(int)0xFF000000},{(int)0x1001311,(int)0xFF00FF00},{(int)0x1001400,(int)0xFF00AF00},{(int)0x1001700,(int)0xFF000000}};;
        std::vector<std::vector<int>> imageButtonColors={{(int)0x1000c00,(int)0xFF00AF00},{(int)0x1000100,(int)0xFF000000},{(int)0x1000101,(int)0xFF00FF00}};   

    private:
        void initSlider1(std::string name,juce::Slider& slider,juce::Label& label,float min,float max,float def);

        juce::ComboBox presetSelected,sequencerMode;
        juce::TextButton savePresetButton,scrollPatterns,debugButton;
        juce::Label presetSelectedLabel,savePresetLabel,sequencerModeLabel,scrollPatternsLabel;

        int counter;
        
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsComponent)
};   

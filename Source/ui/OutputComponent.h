#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

class FxseqAudioProcessorEditor;

class OutputComponent   : public juce::Component
{
    public:
        OutputComponent(FxseqAudioProcessorEditor *ape,std::string ComboTemplate,std::string SliderTemplate,std::string ButtonTemplate);
        void resized() override;

        FxseqAudioProcessorEditor *APE;
        //CALLBACKS
        void skinChange();
        void updateMix();
        void updateGain();

        std::string comboTemplate="combo1",sliderTemplate="slider1",imageButtonTemplate="imagebutton1";
        std::vector<std::vector<int>> comboColors;
        std::vector<std::vector<int>> sliderColors;
        std::vector<std::vector<int>> imageButtonColors;

        juce::Slider outGain,outMix;

    private:
        void initSlider1(std::string name,juce::Slider& slider,juce::Label& label,float min,float max,float def);

        
        juce::Label outGainLabel,outMixLabel;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputComponent)
};   

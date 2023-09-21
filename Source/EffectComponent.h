#pragma once

#include <JuceHeader.h>

class EffectComponent   : public juce::Component
{
    public:
        EffectComponent(int Index,std::string EffectName,std::string ComboTemplate,std::string SliderTemplate);
        void resized() override;
        //CALLBACKS
        void skinChange();

        std::string name;
        int index;
        std::string comboTemplate="combo1";
        std::vector<std::vector<int>> comboColors={{(int)0x1000b00,(int)0xFF000000},{(int)0x1000c00,(int)0xFF00FF00},{(int)0x1000e00,(int)0xFF00FF00},{(int)0x1000a00,(int)0xFF00AF00}};
        std::string sliderTemplate="slider1";
        std::vector<std::vector<int>> sliderColors={{(int)0x1001300,(int)0xFF00FF00},{(int)0x1001312,(int)0xFF000000},{(int)0x1001311,(int)0xFF00FF00},{(int)0x1001400,(int)0xFF00AF00},{(int)0x1001700,(int)0xFF000000}};       
    private:
        void initSlider1(std::string name,juce::Slider& slider,juce::Label& label,float min,float max,float def);

        juce::Slider outGain,outMix,param1,param2,param3;
        juce::ImageButton presetChange;
        juce::Label effectName,outGainLabel,outMixLabel;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectComponent)
};   

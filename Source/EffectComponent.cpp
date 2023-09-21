#include "EffectComponent.h"




EffectComponent::EffectComponent(int Index,std::string EffectName,std::string ComboTemplate,std::string SliderTemplate)
{
    name=EffectName;
    index=Index;
    comboTemplate=ComboTemplate;
    sliderTemplate=SliderTemplate;
    setSize (272, 120);

    initSlider1("Out gain",outGain,outGainLabel,0.0f,2.0f,0.1f);outGain.setValue(1.0f);
    initSlider1("Dry/wet",outMix,outMixLabel,0.0f,1.0f,0.1f);outMix.setValue(1.0f);
}   

void EffectComponent::resized()
{
    outGainLabel.setBounds(195,00,80,20);
    outGain.setBounds     (185,05,80,60);
    outMixLabel.setBounds (195,58,80,20);
    outMix.setBounds      (185,63,80,60);
}

void EffectComponent::skinChange()
{
    for(auto templateColor : sliderColors) {
        outGain.setColour(templateColor[0],juce::Colour(templateColor[1]));
        outMix.setColour(templateColor[0],juce::Colour(templateColor[1]));
    }
    outGainLabel.setColour(0x1000281,juce::Colour(sliderColors[3][1]));
    outMixLabel.setColour(0x1000281,juce::Colour(sliderColors[3][1]));
    
}
////////////////////////////////////////////// TEMPLATES //////////////////////////////////////////////
void EffectComponent::initSlider1(std::string name,juce::Slider& slider,juce::Label& label,float min,float max,float def)
{   
    slider.setSliderStyle (juce::Slider::Rotary);
    slider.setRange (min, max, def);
    slider.setTextBoxStyle (juce::Slider::TextBoxLeft, true, 10, 10);
    label.setText(name,juce::dontSendNotification);

    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}

#include "EffectComponent.h"
#include "PluginEditor.h"



EffectComponent::EffectComponent(int Index,FxseqAudioProcessorEditor *ape,std::string EffectName,std::string ComboTemplate,std::string SliderTemplate,std::string ImageButtonTemplate)
{
    name=EffectName;
    index=Index;
    imageButtonTemplate=ImageButtonTemplate;
    comboTemplate=ComboTemplate;
    sliderTemplate=SliderTemplate;
    setSize (272, 120);
    APE=ape; 

    initSlider1("Out gain",outGain,outGainLabel,0.0f,2.0f,0.1f);outGain.setValue(1.0f);
    initSlider1("Dry/wet",outMix,outMixLabel,0.0f,1.0f,0.1f);outMix.setValue(1.0f);

    programButton.setImages (false, true, true,programButtonImages[programSelected], 1.000f, juce::Colour(programButtonColors[1][1]),juce::Image(), 1.000f, juce::Colour(programButtonColors[0][1]),programButtonImages[programButtonImages.size()-1], 1.000f, juce::Colour(programButtonColors[0][1]));
    programButton.onClick = [this] { changeProgram();};
    addAndMakeVisible(programButton);    

    effectName.setText(name,juce::dontSendNotification);
    addAndMakeVisible(effectName);
}   

void EffectComponent::resized()
{
    effectName.setBounds(90,0,80,20);
    outGainLabel.setBounds(195,00,80,20);
    outGain.setBounds     (185,10,80,60);
    outMixLabel.setBounds (195,58,80,20);
    outMix.setBounds      (185,68,80,60);
    programButton.setBounds(0,0,43,20);
}

void EffectComponent::skinChange()
{
    for(auto templateColor : sliderColors) {
        outGain.setColour(templateColor[0],juce::Colour(templateColor[1]));
        outMix.setColour(templateColor[0],juce::Colour(templateColor[1]));
    }
    outGainLabel.setColour(0x1000281,juce::Colour(sliderColors[3][1]));
    outMixLabel.setColour(0x1000281,juce::Colour(sliderColors[3][1]));
    effectName.setColour(0x1000281,juce::Colour(sliderColors[3][1]));

    programButton.setImages (false, true, true,programButtonImages[programSelected], 1.000f, juce::Colour(programButtonColors[1][1]),juce::Image(), 1.000f, juce::Colour(programButtonColors[0][1]),programButtonImages[programButtonImages.size()-1], 1.000f, juce::Colour(programButtonColors[0][1]));
    
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

/////////////////////////////////////////////// CALLBACKS/////////////////////////////////////////////////
void EffectComponent::changeProgram()
{
    if (programSelected >= programButtonImages.size()-2 ) 
    {
        programSelected=0;
    } else {
        programSelected++;
    }
    programButton.setImages (false, true, true,programButtonImages[programSelected], 1.000f, juce::Colour(programButtonColors[1][1]),juce::Image(), 1.000f, juce::Colour(programButtonColors[1][1]),programButtonImages[programButtonImages.size()-1], 1.000f, juce::Colour(programButtonColors[0][1]));
}

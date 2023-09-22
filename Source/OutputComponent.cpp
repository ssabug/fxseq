#include "OutputComponent.h"
#include "PluginEditor.h" 



OutputComponent::OutputComponent(FxseqAudioProcessorEditor *ape,std::string ComboTemplate,std::string SliderTemplate,std::string ButtonTemplate)
{
    setSize (260, 150);
    
    APE=ape;

    initSlider1("Out gain",outGain,outGainLabel,0.0f,2.0f,0.1f);outGain.setValue(1.0f);
    initSlider1("Dry/wet",outMix,outMixLabel,0.0f,1.0f,0.1f);outMix.setValue(1.0f);
}   

void OutputComponent::resized()
{
    outGainLabel.setBounds(35,30,80,20);
    outGain.setBounds     (30,40,80,80);
    outMixLabel.setBounds (140,30,80,20);
    outMix.setBounds      (130,40,80,80);
}

void OutputComponent::skinChange()
{   
    for(auto templateColor : sliderColors) {
        outGain.setColour(templateColor[0],juce::Colour(templateColor[1]));
        outMix.setColour(templateColor[0],juce::Colour(templateColor[1]));
    }
    outGainLabel.setColour(0x1000281,juce::Colour(sliderColors[3][1]));
    outMixLabel.setColour(0x1000281,juce::Colour(sliderColors[3][1]));

}
////////////////////////////////////////////// TEMPLATES //////////////////////////////////////////////
void OutputComponent::initSlider1(std::string name,juce::Slider& slider,juce::Label& label,float min,float max,float def)
{   
    slider.setSliderStyle (juce::Slider::Rotary);
    slider.setRange (min, max, def);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 80, 20);
    label.setText(name,juce::dontSendNotification);

    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}

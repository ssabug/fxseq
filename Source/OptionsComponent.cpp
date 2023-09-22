#include "OptionsComponent.h"
#include "PluginEditor.h"



OptionsComponent::OptionsComponent(FxseqAudioProcessorEditor *ape,std::string ComboTemplate,std::string SliderTemplate,std::string ImageButtonTemplate)
{
    setSize (260, 150);
    comboTemplate=ComboTemplate;
    sliderTemplate=SliderTemplate;
    imageButtonTemplate=ImageButtonTemplate;
    APE=ape; 
    
    presetSelected.addItem(TRANS("Default"),1);presetSelected.addItem(TRANS("Demo"),2);
    presetSelected.setEditableText (true);
    //for(int i=0;i<comboColors.size();i++) {  presetSelected.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    presetSelected.setSelectedItemIndex(0);
    presetSelected.onChange = [this] {changeSelectedPreset();};
    addAndMakeVisible (presetSelected);
    presetSelectedLabel.setText("Preset",juce::dontSendNotification);
    addAndMakeVisible (presetSelectedLabel);

    savePresetButton.setToggleable(true);
    savePresetButton.onClick = [this] { savePreset();};
    //for(int i=0;i<imageButtonTemplate.size();i++) {  savePresetButton.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));   } // CRASHES THE PLUGIN!!!
    savePresetLabel.setText("Save",juce::dontSendNotification);
    addAndMakeVisible (savePresetButton);
    addAndMakeVisible (savePresetLabel);
}   

void OptionsComponent::resized()
{
    presetSelected.setBounds        (10,15,150,20);
    presetSelectedLabel.setBounds   (50,00,80,20);
    savePresetButton.setBounds            (180,15,40,20);
    savePresetLabel.setBounds       (177,0,40,20);    
}

void OptionsComponent::skinChange()
{   
    for(int i=0;i<comboColors.size();i++) {  presetSelected.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    //for(int i=0;i<imageButtonTemplate.size();i++) {  savePresetButton.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));   } // CRASHES THE PLUGIN!!!

    presetSelectedLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));
    savePresetLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));
}
////////////////////////////////////////////// TEMPLATES //////////////////////////////////////////////
void OptionsComponent::initSlider1(std::string name,juce::Slider& slider,juce::Label& label,float min,float max,float def)
{   
    slider.setSliderStyle (juce::Slider::Rotary);
    slider.setRange (min, max, def);
    slider.setTextBoxStyle (juce::Slider::TextBoxLeft, true, 10, 10);
    label.setText(name,juce::dontSendNotification);

    addAndMakeVisible(slider);
    addAndMakeVisible(label);
}
/////////////////////////////////////////////// CALLBACKS///////////////////////////////////////////////
void OptionsComponent::changeSelectedPreset()
{

}

void OptionsComponent::savePreset()
{

}

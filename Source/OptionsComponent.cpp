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
    for(int i=0;i<comboColors.size();i++) {  presetSelected.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    presetSelected.setSelectedItemIndex(0);
    presetSelected.onChange = [this] {changeSelectedPreset();};
    addAndMakeVisible (presetSelected);
    presetSelectedLabel.setText("Preset",juce::dontSendNotification);
    addAndMakeVisible (presetSelectedLabel);

    savePresetButton.setToggleable(false);
    savePresetButton.onClick = [this] { savePreset();};
    for(int i=0;i<imageButtonColors.size();i++) {  savePresetButton.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));   } // CRASHES THE PLUGIN!!!
    savePresetLabel.setText("Save",juce::dontSendNotification);
    addAndMakeVisible (savePresetButton);
    addAndMakeVisible (savePresetLabel);

    sequencerMode.addItem(TRANS("Pattern"),1);sequencerMode.addItem(TRANS("Sequence"),2);
    for(int i=0;i<comboColors.size();i++) {  sequencerMode.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    sequencerMode.setSelectedItemIndex(0);
    sequencerMode.onChange = [this] {changeSequencerMode();};
    addAndMakeVisible (sequencerMode);
    sequencerModeLabel.setText("Mode",juce::dontSendNotification);
    addAndMakeVisible (sequencerModeLabel);

    scrollPatterns.setToggleable(true);
    scrollPatterns.onClick = [this] { switchScroll();};
    for(int i=0;i<imageButtonColors.size();i++) {  scrollPatterns.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));   } // CRASHES THE PLUGIN!!!
    scrollPatternsLabel.setText("Autoscroll",juce::dontSendNotification);
    addAndMakeVisible (scrollPatterns);
    addAndMakeVisible (scrollPatternsLabel);
}   

void OptionsComponent::resized()
{
    presetSelected.setBounds        (10,15,150,20);
    presetSelectedLabel.setBounds   (50,00,80,20);
    savePresetButton.setBounds      (180,15,40,20);
    savePresetLabel.setBounds       (177,0,40,20);
    sequencerModeLabel.setBounds    (40,50,50,20);
    sequencerMode.setBounds         (30,65,100,20);    
    scrollPatternsLabel.setBounds   (140,50,80,20);
    scrollPatterns.setBounds        (160,65,40,20);    
}

void OptionsComponent::skinChange()
{   
    for(int i=0;i<comboColors.size();i++) {  
        presetSelected.setColour(comboColors[i][0],juce::Colour(comboColors[i][1])); 
        sequencerMode.setColour(comboColors[i][0],juce::Colour(comboColors[i][1])); 
    }
    for(int i=0;i<imageButtonColors.size();i++) {
        savePresetButton.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));// CRASHES THE PLUGIN!!!
        scrollPatterns.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));    
     } 

    presetSelectedLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));
    savePresetLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));
    sequencerModeLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));
    scrollPatternsLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));

    if (scroll) { scrollPatterns.setColour(juce::TextButton::buttonColourId,juce::Colours::black);}
    else {scrollPatterns.setColour(juce::TextButton::buttonColourId,juce::Colour(imageButtonColors[0][1]));}
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

void OptionsComponent::changeSequencerMode()
{
    sequenceMode=sequencerMode.getSelectedItemIndex();
    APE->changeSequenceMode(sequenceMode); 
}

void OptionsComponent::switchScroll()
{
    if (scroll) {
        scroll=false;
        scrollPatterns.setColour(juce::TextButton::buttonColourId,juce::Colours::black);}
    else {
        scroll=true;
        scrollPatterns.setColour(juce::TextButton::buttonColourId,juce::Colour(imageButtonColors[0][1]));
    }
}

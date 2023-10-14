#include "OptionsComponent.h"
#include "../PluginEditor.h"



OptionsComponent::OptionsComponent(FxseqAudioProcessorEditor *ape,std::string ComboTemplate,std::string SliderTemplate,std::string ImageButtonTemplate)
{
    setSize (260, 150);
    comboTemplate=ComboTemplate;
    sliderTemplate=SliderTemplate;
    imageButtonTemplate=ImageButtonTemplate;
    APE=ape; 
    
    auto presetList=APE->getPresetList();
    for(int i=0;i<presetList.size();i++) 
    {
        auto preset=presetList[i];
        /*presetSelected.addItem(TRANS("Default"),1);presetSelected.addItem(TRANS("Demo"),2);*/
        preset=preset.substr(0,preset.size()-4);
        int begin=APE->getPath("presets").size();
        preset=preset.substr(begin);
        presetSelected.addItem(TRANS(preset),i+1);
    }
    presetSelected.setEditableText (true);
    for(int i=0;i<comboColors.size();i++) {  presetSelected.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    presetSelected.setSelectedItemIndex(0,juce::dontSendNotification);
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

    for (int i=0;i<8;i++){patternSelect.addItem(TRANS(std::to_string(i+1)),i+1);}
    for(int i=0;i<comboColors.size();i++) {  patternSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    patternSelect.setSelectedItemIndex(0);
    addAndMakeVisible (patternSelect);

    copy.setButtonText("copy");
    paste.setButtonText("paste");
    clear.setButtonText("clear");
    reset.setButtonText("reset");

    copy.onClick = [this] {patternUtils("copy");};
    paste.onClick = [this] {patternUtils("paste");};
    clear.onClick = [this] {patternUtils("clear");};
    reset.onClick = [this] {patternUtils("reset");};

    addAndMakeVisible (copy);
    addAndMakeVisible (paste);
    addAndMakeVisible (clear);
    addAndMakeVisible (reset);

    debugButton.onClick = [this] {debugFunction();};
    addAndMakeVisible (debugButton);
}   

void OptionsComponent::resized()
{
    presetSelected.setBounds        (10,15,150,20);
    presetSelectedLabel.setBounds   (50,00,80,20);
    savePresetButton.setBounds      (180,15,40,20);
    savePresetLabel.setBounds       (177,0,40,20);

    sequencerModeLabel.setBounds    (20,40,50,20);
    sequencerMode.setBounds         (10,55,100,20);    
    scrollPatternsLabel.setBounds   (160,40,80,20);
    scrollPatterns.setBounds        (180,55,40,20); 

    debugButton.setBounds           (180,100,40,20); 
  
    patternSelect.setBounds         (15,100,50,20);
    copy.setBounds                  (75,85,40,20);
    paste.setBounds                 (75,115,40,20);
    clear.setBounds                 (120,85,40,20);
    reset.setBounds                 (120,115,40,20);
  
}

void OptionsComponent::skinChange()
{   
    for(int i=0;i<comboColors.size();i++) {  
        presetSelected.setColour(comboColors[i][0],juce::Colour(comboColors[i][1])); 
        sequencerMode.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));
        patternSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1])); 
    }
    for(int i=0;i<imageButtonColors.size();i++) {
        savePresetButton.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));
        scrollPatterns.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));
        copy.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));
        paste.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));
        clear.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));
        reset.setColour(imageButtonColors[i][0],juce::Colour(imageButtonColors[i][1]));  
     }

    savePresetButton.setColour(juce::ComboBox::outlineColourId,juce::Colour(comboColors[2][1]));
    scrollPatterns.setColour(juce::ComboBox::outlineColourId,juce::Colour(comboColors[2][1]));
    copy.setColour(juce::ComboBox::outlineColourId,juce::Colour(comboColors[2][1]));
    paste.setColour(juce::ComboBox::outlineColourId,juce::Colour(comboColors[2][1]));
    clear.setColour(juce::ComboBox::outlineColourId,juce::Colour(comboColors[2][1]));
    reset.setColour(juce::ComboBox::outlineColourId,juce::Colour(comboColors[2][1]));   

    presetSelectedLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));
    savePresetLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));
    sequencerModeLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));
    scrollPatternsLabel.setColour(0x1000281,juce::Colour(comboColors[1][1]));

    sequencerMode.setSelectedItemIndex(int(APE->getMasterParam("sequencerMode")));

    if ( !scroll) { scrollPatterns.setColour(juce::TextButton::buttonColourId,juce::Colours::black);}
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
    //std::string presetText = APE->removeForbiddenCharacters(presetSelected.getText().toStdString());
    //presetSelected.setText(presetText); 
    APE->loadPreset(presetSelected.getText().toStdString());
}

void OptionsComponent::savePreset()
{
    std::string presetText = APE->removeForbiddenCharacters(presetSelected.getText().toStdString());
    
    if (presetText  != "" ) {
        //writeXMLPreset("/home/pwner/.ssabug/choppah/presets/test.xml");    
        if ( !APE->presetExists(presetText)) {
            presetSelected.addItem (TRANS(presetText), presetSelected.getNumItems()+1);
        } 
        APE->saveXMLPreset(presetText);
    }
}

void OptionsComponent::changeSequencerMode()
{
    sequenceMode=(bool)sequencerMode.getSelectedItemIndex();
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

void OptionsComponent::debugFunction()
{

    /*std::vector<float> values={0.5f,1.0f,2.0f,4.0f};

    APE->debugRepeater(values[debugCounter]);


    if (debugCounter>=values.size()-1) 
    {
        debugCounter=0;
    } else {
        debugCounter++;
    }*/
    //APE->saveXMLPreset();
    //fileName="/home/pwner/dev/fxseq/Ressources/presets/default.xml";
    APE->loadPreset(APE->getPath("presets") + presetSelected.getText().toStdString() + ".xml");
}

void OptionsComponent::patternUtils(std::string action)
{
    APE->patternUtils(action,patternSelect.getSelectedItemIndex());
}

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

    skinPath=APE->getPath("currentSkin");
    imagePath=APE->getPath("images");

    initSlider1("Out gain",outGain,outGainLabel,0.0f,2.0f,0.1f);
    initSlider1("Dry/wet",outMix,outMixLabel,0.0f,1.0f,0.1f);
    outGainLabel.setFont (12.0f);
    outMixLabel.setFont (12.0f);

    programButton.setImages (false, true, true,programButtonImages[programSelected], 1.000f, juce::Colour(programButtonColors[1][1]),juce::Image(), 1.000f, juce::Colour(programButtonColors[0][1]),programButtonImages[programButtonImages.size()-1], 1.000f, juce::Colour(programButtonColors[0][1]));
    programButton.onClick = [this] { changeProgram();};
    addAndMakeVisible(programButton);    

    effectName.setText(name,juce::dontSendNotification);
    effectName.setFont (juce::Font (20.0f, juce::Font::bold));
    addAndMakeVisible(effectName);

    for (int i=0;i<sizeof(params)/sizeof(params[0]);i++)
    {
        std::vector<std::string> range=(APE->getFxParamProperty(index,i,0,"range"));//APE->getFxParamRange(index,i);
        initSlider1("param"+std::__cxx11::to_string(i),params[i],paramsLabel[i],std::stof(range[0]),std::stof(range[1]),std::stof(range[2]));
        params[i].setTextBoxStyle (juce::Slider::TextBoxBelow, true, 10, 10);

        std::string paramName = (APE->getFxParamProperty(index,i,0,"name"))[0];//APE->getFxParameterName(index,i);
        if (paramName == "") {params[i].setVisible(0); paramsLabel[i].setVisible(0);} 
        if (paramName == "Frequency") {params[i].setNormalisableRange(juce::NormalisableRange<double>(std::stof(range[0]),std::stof(range[1]),std::stof(range[2]), 0.2f));}

        paramsLabel[i].setText(paramName,juce::dontSendNotification);
    
    }
    
    params[0].onValueChange = [this] { changeParam(0);};
    params[1].onValueChange = [this] { changeParam(1);};
    params[2].onValueChange = [this] { changeParam(2);};

}   

void EffectComponent::resized()
{
    effectName.setBounds  (90,0,100,20);
    outGainLabel.setBounds(200,00,80,20);
    outGain.setBounds     (185,10,80,60);
    outMixLabel.setBounds (200,58,80,20);
    outMix.setBounds      (185,68,80,60);
    programButton.setBounds(0,0,43,20);

    
    for (int i=0;i<sizeof(params)/sizeof(params[0]);i++)
    {
        paramsLabel[i].setBounds(i*65,35,60,20);
        params[i].setBounds     (i*65,40,60,90);

        if (paramsLabel[i].getText() != "") {
            float paramValue=std::stof((APE->getFxParamProperty(index,i,programSelected,"value")[0]));
            if (paramValue != -100.0f) {
                params[i].setValue(paramValue);
            }
        }

    }
    
}

void EffectComponent::skinChange()
{
    for(auto templateColor : sliderColors) {
        outGain.setColour(templateColor[0],juce::Colour(templateColor[1]));
        outMix.setColour(templateColor[0],juce::Colour(templateColor[1]));
        for (int i=0;i<sizeof(params)/sizeof(params[0]);i++) { params[i].setColour(templateColor[0],juce::Colour(templateColor[1])); }
    }

    for (int i=0;i<sizeof(params)/sizeof(params[0]);i++) 
    {
        paramsLabel[i].setColour(0x1000281,juce::Colour(sliderColors[3][1]));    
    }
                   
    outGainLabel.setColour(0x1000281,juce::Colour(sliderColors[3][1]));
    outMixLabel.setColour(0x1000281,juce::Colour(sliderColors[3][1]));
    effectName.setColour(0x1000281,juce::Colour(sliderColors[3][1]));

    programButton.setImages (false, true, true,programButtonImages[programSelected], 1.000f, juce::Colour(programButtonColors[1][1]),juce::Image(), 1.000f, juce::Colour(programButtonColors[0][1]),programButtonImages[programButtonImages.size()-1], 1.000f, juce::Colour(programButtonColors[0][1]));
    

}


void EffectComponent::changeParam(int paramIndex)
{   
    APE->updateFxParam(index,programSelected+1,paramIndex,params[paramIndex].getValue());   
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

    for (int i=0;i<sizeof(params)/sizeof(params[0]);i++) 
    {
        if (  paramsLabel[i].getText() != "" ) {
            //debug="fxIndex " + std::__cxx11::to_string(index) + " programIndex " + std::__cxx11::to_string(programSelected) + " paramIndex " + std::__cxx11::to_string(i);
            params[i].setValue(std::stof((APE->getFxParamProperty(index,i,programSelected,"value")[0])));//(APE->getFxParamValue(index,programSelected,i));
        }
    }
     
}

void EffectComponent::hidePrograms()
{
    if (programButton.isVisible()) {    programButton.setVisible(false); }
}

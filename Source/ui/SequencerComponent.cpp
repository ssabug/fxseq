/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "SequencerComponent.h"
#include "../PluginEditor.h" 

SequencerComponent::SequencerComponent(int Index,FxseqAudioProcessorEditor *ape,std::string StepSeqTemplate,std::string ComboTemplate)
{
    APE=ape;
    index=Index;
    position=index;//APE->getSequencerPosition(index);audioProcessor.updateParameter(fxNamesStr[i]+"_position",(float)sequencers[i].position); 
    stepSeqTemplate=StepSeqTemplate;
    comboTemplate=ComboTemplate;
    skinPath=APE->getPath("currentSkin");
    imagePath=APE->getPath("images");

    juce::Colour color_stepseq_1=juce::Colour(stepSeqColors[0][1]);
    juce::Colour color_stepseq_2=juce::Colour(stepSeqColors[1][1]);
    
    setSize (1200, 80);

    for (int i=0;i<stepCount;i++)
    {
        seqStep[i].setToggleable(true);
        seqStep[i].setImages (false, true, true,stepSeqImages[0], 1.000f, color_stepseq_1,juce::Image(), 1.000f, color_stepseq_2,stepSeqImages[1], 1.000f, color_stepseq_1);
        seqStep[i].setAlwaysOnTop(true);
        addAndMakeVisible (seqStep[i]);
    }
    for (int i=0;i<stepCount/4;i++)
    {
        seqStepMkr[i].setText("______", juce::dontSendNotification);
        seqStepMkr[i].setFont (juce::Font (15.0f, juce::Font::bold));
        addAndMakeVisible (seqStepMkr[i]);
    }
    seqStep[0].onClick = [this] { seqStepClick(0);};seqStep[4].onClick = [this] { seqStepClick(4);};seqStep[8].onClick = [this] { seqStepClick(8);};seqStep[12].onClick = [this] { seqStepClick(12);};
    seqStep[1].onClick = [this] { seqStepClick(1);};seqStep[5].onClick = [this] { seqStepClick(5);};seqStep[9].onClick = [this] { seqStepClick(9);};seqStep[13].onClick = [this] { seqStepClick(13);};
    seqStep[2].onClick = [this] { seqStepClick(2);};seqStep[6].onClick = [this] { seqStepClick(6);};seqStep[10].onClick = [this] { seqStepClick(10);};seqStep[14].onClick = [this] { seqStepClick(14);};
    seqStep[3].onClick = [this] { seqStepClick(3);};seqStep[7].onClick = [this] { seqStepClick(7);};seqStep[11].onClick = [this] { seqStepClick(11);};seqStep[15].onClick = [this] { seqStepClick(15);};

    for(int i=0;i<16;i++) {patternSelect.addItem(TRANS(std::__cxx11::to_string(i+1)),i+1);}
    for(int i=0;i<comboColors.size();i++) {  patternSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    patternSelect.setSelectedItemIndex(0);
    patternSelect.onChange = [this] {changeSelectedPattern();};
    addAndMakeVisible (patternSelect);
    patternSelectLabel.setText("PATTERN",juce::dontSendNotification);
    patternSelectLabel.setFont (9.0f);
    addAndMakeVisible (patternSelectLabel);

    for(int i=0;i<8;i++) {clockMultSelect.addItem(TRANS(std::__cxx11::to_string((int)pow(2,i))),i+1);}
    for(int i=0;i<comboColors.size();i++) {  clockMultSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    clockMultSelect.setSelectedItemIndex(2);
    clockMultSelect.onChange = [this] {changeClockMult();};
    addAndMakeVisible (clockMultSelect);
    clockMultSelectLabel.setText("CLOCK",juce::dontSendNotification);
    clockMultSelectLabel.setFont (9.0f);
    addAndMakeVisible (clockMultSelectLabel);


    effectTypeSelect.addItem(TRANS(APE->fxNamesStr[index]).toUpperCase(),1);
    for(int i=0;i<comboColors.size();i++) {  effectTypeSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    effectTypeSelect.setSelectedItemIndex(0);
    effectTypeSelect.onChange = [this] {changeEffect();};
    addAndMakeVisible (effectTypeSelect);
    //effectTypeSelect.setEnabled(false);

    
    seqMoveUpButton.onClick = [this] {changeSequencerPosition(true);};
    seqMoveDownButton.onClick = [this] {changeSequencerPosition(false);};
    addAndMakeVisible (seqMoveUpButton);
    addAndMakeVisible (seqMoveDownButton);
    
}

void SequencerComponent::resized()
{
    int y0=5;
    for (int i=0;i<stepCount;i++)
    {
        seqStep[i].setBounds (150+i*60, y0, 46, 28);
    }
    for (int i=0;i<stepCount/4;i++)
    {
        seqStepMkr[i].setBounds(150+i*60*4,y0+18/*37*/,46,16);
    }
    effectTypeSelect.setBounds    (15,0,105,16);
    patternSelectLabel.setBounds  (15,y0+11,60,10);
    patternSelect.setBounds       (15,y0+20,50,14);   
    clockMultSelectLabel.setBounds(75,y0+11,60,10);
    clockMultSelect.setBounds     (70,y0+20,50,14);
    seqMoveUpButton.setBounds     (0,y0,10,10);
    seqMoveDownButton.setBounds   (0,y0+20,10,10);
    
    
}
/////////////////////////////////////////////////////////////// CALLBACKS ///////////////////////////////////////////////////////////////
void SequencerComponent::skinChange()
{

    juce::Colour color_stepseq_1=juce::Colour(stepSeqColors[0][1]);
    juce::Colour color_stepseq_2=juce::Colour(stepSeqColors[1][1]);
   
    for(int i=0;i<stepCount;i++)
    {
        seqStep[i].setImages (false, true, true,stepSeqImages[0], 1.000f, color_stepseq_1,juce::Image(), 1.000f, color_stepseq_2,stepSeqImages[1], 1.000f, color_stepseq_1);
    }
    for (int i=0;i<stepCount/4;i++)
    {
        seqStepMkr[i].setColour(0x1000281,color_stepseq_2);
    }

    for(int i=0;i<comboColors.size();i++) 
    {  
        patternSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));
        clockMultSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));
        effectTypeSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));
    }
    patternSelectLabel.setColour(0x1000281,color_stepseq_1);
    clockMultSelectLabel.setColour(0x1000281,color_stepseq_1);

    seqMoveUpButton.setColour(0x1000c00,color_stepseq_2);
    seqMoveUpButton.setColour(0x1000100,juce::Colours::black);
    seqMoveUpButton.setColour(0x1000101,color_stepseq_2);

    seqMoveDownButton.setColour(0x1000c00,color_stepseq_2);
    seqMoveDownButton.setColour(0x1000100,juce::Colours::black);
    seqMoveDownButton.setColour(0x1000101,color_stepseq_2);

    position=(int)(APE->getMasterParam(APE->fxNamesStr[index]+"_position"))+1; 
    clockMultSelect.setSelectedItemIndex(int(APE->getMasterParam(APE->fxNamesStr[index]+"_clockDiv")));
    patternSelect.setSelectedItemIndex(int(APE->getMasterParam(APE->fxNamesStr[index]+"_pattern")));
}

void SequencerComponent::seqStepClick(int stepIndex)
{
    if (pattern[stepIndex]< stepMaxValue) { pattern[stepIndex]++;}
    else {pattern[stepIndex]=0;}

    APE->updateProcessorPattern(index,patternSelect.getSelectedItemIndex());
}

void SequencerComponent::updatePosition(int position)
{
    juce::Colour color_stepseq_1=juce::Colour(stepSeqColors[0][1]);
    juce::Colour color_stepseq_2=juce::Colour(stepSeqColors[1][1]);
    juce::Colour color1,color2;

    for (int i=0;i<stepCount;i++)
    {
        if (i==position) {
            color1=color_stepseq_1;
            color2=color_stepseq_2;
        } else {
            color1=color_stepseq_2;
            color2=color_stepseq_1;
        }

        seqStep[i].setImages (false, true, true,stepSeqImages[pattern[i]], 1.000f, color1,juce::Image(), 1.000f, color_stepseq_1,stepSeqImages[stepSeqImages.size()-1], 1.000f, color2);        
    }
}

void SequencerComponent::updatePattern(int patternData[]) {
    for ( int i=0;i<stepCount;i++)
    {
        pattern[i]=patternData[i];
    }
}

void SequencerComponent::remoteChangeSelectedPattern(int patternToSelect)
{
    patternSelect.setSelectedItemIndex(patternToSelect);
}

void SequencerComponent::changeSelectedPattern()
{
    APE->updateSeqPattern(index,patternSelect.getSelectedItemIndex());
    //APE->updateSelectedProcessorPattern(index,patternSelect.getSelectedItemIndex());
}

void SequencerComponent::changeClockMult()
{
    clockMult=pow(2,clockMultSelect.getSelectedItemIndex());
    //APE->updateSelectedProcessorClock(index,clockMultSelect.getSelectedItemIndex());
}

void SequencerComponent::changeEffect()
{
    //clockMult=std::stoi(clockMultSelect.getItemText(clockMultSelect.getSelectedItemIndex()).toStdString());
    //APE->updateSelectedProcessorEffect(index,effectTypeSelect.getSelectedItemIndex());
}

int SequencerComponent::getSelectedPattern()
{
    return patternSelect.getSelectedItemIndex();
}

void SequencerComponent::changeSequencerPosition(bool up)
{
    int newPosition=position;
    if (up) {
        if (position>0) {newPosition--;}
        else {newPosition=APE->getSequencerCount()-1;};
    } else {
        if (position<APE->getSequencerCount()-1) {newPosition++;}
        else {newPosition=0;};
    }
    APE->changeFxPosition(index,newPosition);
}


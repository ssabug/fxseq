/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FxseqAudioProcessorEditor::FxseqAudioProcessorEditor (FxseqAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    /*sequencers[0]=new SequencerComponent(this);
    sequencers[1]=new SequencerComponent(this);*/

    setSize (1000, 400);

    
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {
        updateSeqPattern(i,0); 
        addAndMakeVisible(sequencers[i]);
    }
  

    addAndMakeVisible(debugLog);

    startTimerHz(50);
}

FxseqAudioProcessorEditor::~FxseqAudioProcessorEditor()
{
}

//==============================================================================
void FxseqAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void FxseqAudioProcessorEditor::resized()
{
    
    debugLog.setBounds(5,200,300,195);

    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {
        sequencers[i].setBounds(0,i*40,1000,80);
    }
}

void FxseqAudioProcessorEditor::timerCallback()
{    
    double ppq=audioProcessor.ppq;
    int clockMult=pow(2,2);
    float unused;
    int positions[2]={int(std::floor( std::modf(ppq/clockMult,&unused)*(16))),
                      int(std::floor( std::modf(ppq/(clockMult*2),&unused)*(16)))
                     };

    debugLog.setText(  /* "sequencer1_stepCount " + std::__cxx11::to_string(sequencers[0].stepCount) + "\n"
                       +"sequencer1_stepMaxValue " + std::__cxx11::to_string(sequencers[0].stepMaxValue) + "\n"  
                       +"sequencer1_getNumChildComponents ()  " + std::__cxx11::to_string(sequencers[0].getNumChildComponents () ) + "\n"*/
                        "ppq " + std::__cxx11::to_string(ppq) + "\n"
                       +"position1 " + std::__cxx11::to_string(positions[0]) + "\n"
                       +"position2 " + std::__cxx11::to_string(positions[1]) 
                    );

    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {
        sequencers[i].updatePosition(positions[i]);
    }

}

void FxseqAudioProcessorEditor::updateSeqPattern(int sequencerIndex,int patternIndex)
{
    sequencers[sequencerIndex].updatePattern(audioProcessor.seq1Patterns[patternIndex]);
}

void FxseqAudioProcessorEditor::updateProcessorPattern(int sequencerIndex,int patternIndex) 
{
    if (sequencerIndex == 0 ){
        for (int i=0;i<sequencers[sequencerIndex].stepCount;i++)
        {
            audioProcessor.seq1Patterns[patternIndex][i]=sequencers[sequencerIndex].pattern[i];
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////// SEQUENCER /////////////////////////////////////////////////////////////////////////////////////
SequencerComponent::SequencerComponent(/*juce::AudioProcessorEditor *ape*/)
{
    juce::Colour color_stepseq_1=juce::Colour(0xFF00FF00);
    juce::Colour color_stepseq_2=juce::Colour(0xFF007F00);
    std::vector<juce::Image> images={juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_off.png")),
                                     juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_on.png"))
                                     };
    //APE=ape; 
    setSize (400, 80);

    for (int i=0;i<stepCount;i++)
    {
        seqStep[i].setToggleable(true);
        seqStep[i].setImages (false, true, true,img_stepSeqOff, 1.000f, color_stepseq_1,juce::Image(), 1.000f, color_stepseq_2,img_stepSeqOn, 1.000f, color_stepseq_1);
        seqStep[i].setAlwaysOnTop(true);
        addAndMakeVisible (seqStep[i]);
        //this->addChildComponent(seqStep[i],0);
    }
    seqStep[0].onClick = [this] { seqStepClick(0);};seqStep[4].onClick = [this] { seqStepClick(4);};seqStep[8].onClick = [this] { seqStepClick(8);};seqStep[12].onClick = [this] { seqStepClick(12);};
    seqStep[1].onClick = [this] { seqStepClick(1);};seqStep[5].onClick = [this] { seqStepClick(5);};seqStep[9].onClick = [this] { seqStepClick(9);};seqStep[13].onClick = [this] { seqStepClick(13);};
    seqStep[2].onClick = [this] { seqStepClick(2);};seqStep[6].onClick = [this] { seqStepClick(6);};seqStep[10].onClick = [this] { seqStepClick(10);};seqStep[14].onClick = [this] { seqStepClick(14);};
    seqStep[3].onClick = [this] { seqStepClick(3);};seqStep[7].onClick = [this] { seqStepClick(7);};seqStep[11].onClick = [this] { seqStepClick(11);};seqStep[15].onClick = [this] { seqStepClick(15);};
}
void SequencerComponent::resized()
{
    for (int i=0;i<stepCount;i++)
    {
        seqStep[i].setBounds (5+i*60, 5, 46, 16);
    }
    
}
void SequencerComponent::seqStepClick(int stepIndex)
{
    if (pattern[stepIndex]< stepMaxValue) { pattern[stepIndex]++;}
    else {pattern[stepIndex]=0;}

    //updateProcessorPattern(0,0);
}

void SequencerComponent::updatePosition(int position)
{
    juce::Colour color_stepseq_1=juce::Colour(0xFF00FF00),color_stepseq_2=juce::Colour(0xFF007F00);
    juce::Colour color1,color2;
    //int clockMult=pow(2,1);
    //int position=int(std::floor( std::modf(ppq/clockMult,nullptr)*(stepCount)));
    std::vector<juce::Image> images={juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_off.png")),
                                     juce::ImageFileFormat::loadFrom(juce::File(imagePath+"stepseq_on.png"))
                                     };

    for (int i=0;i<stepCount;i++)
    {
        if (i==position) {
            color1=color_stepseq_1;
            color2=color_stepseq_2;
        } else {
            color1=color_stepseq_2;
            color2=color_stepseq_1;
        }

        seqStep[i].setImages (false, true, true,images[pattern[i]], 1.000f, color1,juce::Image(), 1.000f, color1,images[images.size()-1], 1.000f, color2);        
    }
}

void SequencerComponent::updatePattern(int patternData[]) {
    for ( int i=0;i<stepCount;i++)
    {
        pattern[i]=patternData[i];
    }
}

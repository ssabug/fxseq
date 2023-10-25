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
    setSize (1400, 800);
    
    //fxNamesStr=audioProcessor.fxNamesStr;

    //initDirectories();

    std::string skinFilePath=getPath("currentSkinFile");
    std::string imagePath=getPath("images");

    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {
        std::vector<std::string> images=readXMLVectorParam(skinFilePath,"skin/templates/"+sequencers[i].stepSeqTemplate+"/images");
        sequencers[i].stepSeqColors=readXMLVector2Param(skinFilePath,"skin/templates/"+sequencers[i].stepSeqTemplate+"/colors");
        sequencers[i].comboColors=readXMLVector2Param(skinFilePath,"skin/templates/"+sequencers[i].comboTemplate+"/colors");

        sequencers[i].stepSeqImages.clear();
        for (int j=0;j<images.size();j++) {   sequencers[i].stepSeqImages.push_back(juce::ImageFileFormat::loadFrom(juce::File(imagePath+images[j])));    }
        sequencers[i].stepMaxValue=sequencers[i].stepSeqImages.size()-2;
        //patternSelect,clockMultSelect
        patternSelectAttachement[i].reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (audioProcessor.pluginParameters, fxNamesStr[i] +  "_pattern", sequencers[i].patternSelect));
        clockMultSelectAttachment[i].reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (audioProcessor.pluginParameters, fxNamesStr[i] +  "_clockDiv", sequencers[i].clockMultSelect)); 
        updateSeqPattern(i,sequencers[i].getSelectedPattern());
        sequencers[i].skinChange();
        addAndMakeVisible(sequencers[i]);
    }
  
    for (int i=0;i<sizeof(effects)/sizeof(effects[0]);i++) {
        effects[i].comboColors=readXMLVector2Param(skinFilePath,"skin/templates/"+effects[i].comboTemplate+"/colors");
        effects[i].sliderColors=readXMLVector2Param(skinFilePath,"skin/templates/"+effects[i].sliderTemplate+"/colors");
        effects[i].programButtonColors=readXMLVector2Param(skinFilePath,"skin/templates/"+effects[i].imageButtonTemplate+"/colors");
        std::vector<std::string> imagesP=readXMLVectorParam(skinFilePath,"skin/templates/"+effects[i].imageButtonTemplate+"/images");
        effects[i].programButtonImages.clear();
        for (int j=0;j<imagesP.size();j++) {   effects[i].programButtonImages.push_back(juce::ImageFileFormat::loadFrom(juce::File(imagePath+imagesP[j])));    }
        
        fxOutMixAttachement[i].reset (new juce::AudioProcessorValueTreeState::SliderAttachment (audioProcessor.pluginParameters, fxNamesStr[i] + "_dry/wet", effects[i].outMix));
        fxOutGainAttachement[i].reset (new juce::AudioProcessorValueTreeState::SliderAttachment (audioProcessor.pluginParameters,fxNamesStr[i] +  "_gain", effects[i].outGain));
        effects[i].skinChange();
        addAndMakeVisible(effects[i]);
    }

    options.comboColors=readXMLVector2Param(skinFilePath,"skin/templates/"+options.comboTemplate+"/colors");
    options.sliderColors=readXMLVector2Param(skinFilePath,"skin/templates/"+options.sliderTemplate+"/colors");
    options.imageButtonColors=readXMLVector2Param(skinFilePath,"skin/templates/"+options.imageButtonTemplate+"/colors");
    sequencerModeAttachment.reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (audioProcessor.pluginParameters,"sequencerMode", options.sequencerMode)); 
    options.skinChange();
    addAndMakeVisible(options);

    output.comboColors=readXMLVector2Param(skinFilePath,"skin/templates/"+output.comboTemplate+"/colors");
    output.sliderColors=readXMLVector2Param(skinFilePath,"skin/templates/"+output.sliderTemplate+"/colors");
    output.imageButtonColors=readXMLVector2Param(skinFilePath,"skin/templates/"+output.imageButtonTemplate+"/colors");
    outMixAttachement.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (audioProcessor.pluginParameters, "drywet", output.outMix));
    outGainAttachement.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (audioProcessor.pluginParameters, "outgain", output.outGain));
    output.skinChange();
    addAndMakeVisible(output);

    sequenceSeq.comboColors=readXMLVector2Param(skinFilePath,"skin/templates/"+sequenceSeq.comboTemplate+"/colors");
    sequenceSeq.sliderColors=readXMLVector2Param(skinFilePath,"skin/templates/"+sequenceSeq.sliderTemplate+"/colors");
    sequenceSeq.imageButtonColors=readXMLVector2Param(skinFilePath,"skin/templates/"+sequenceSeq.imageButtonTemplate+"/colors");
    sequenceSelectedAttachment.reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (audioProcessor.pluginParameters, "sequenceNumber", sequenceSeq.sequenceSelect));
    sequenceLengthAttachment.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (audioProcessor.pluginParameters, "sequenceLength", sequenceSeq.sequenceLengthSlider));
    sequenceSeq.skinChange();
    addAndMakeVisible(sequenceSeq);

    addAndMakeVisible(debugLog);
    
    juce::Colour logoColour1=juce::Colour(sequenceSeq.comboColors[1][1]);
    juce::Colour logoColour2= juce::Colour(sequenceSeq.comboColors[1][1] & 0xFF7F7F7F);
    juce::Image img=juce::ImageFileFormat::loadFrom(juce::File(getPath("images")+"logo_1.png"));
    logo.setImages (false, true, true,img, 1.000f,logoColour2 ,juce::Image(), 1.000f, logoColour1,img, 1.000f, logoColour1);
    addAndMakeVisible(logo);
    
    startTimerHz(30);
}

FxseqAudioProcessorEditor::~FxseqAudioProcessorEditor()
{
}

//==============================================================================
void FxseqAudioProcessorEditor::paint (juce::Graphics& g)
{
    int seqHeigth=60;
    int seqsSize=(sizeof(sequencers)/sizeof(sequencers[0])-1)*seqHeigth;    

    g.fillAll (juce::Colours::black);
    g.setFont (15.0f);
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {
        g.setColour (juce::Colour(sequencers[i].stepSeqColors[1][1]));
        g.drawRect (juce::Rectangle(0,sequencers[i].position*seqHeigth,140,seqHeigth-10), 2);
        g.drawRect (juce::Rectangle(0,sequencers[i].position*seqHeigth,1120,seqHeigth-10), 2);
        if (i<4)
        {g.drawRect (juce::Rectangle(i*282,seqsSize+110,272,130), 2);}
        else {g.drawRect (juce::Rectangle((i-4)*282,/*370*/seqsSize+110+140,272,130), 2);};
    }
    g.setColour (juce::Colour(0xFF3F3F7F));
    g.drawRect     (juce::Rectangle(0,seqsSize+60,140,40), 2);
    g.drawRect     (juce::Rectangle(0,seqsSize+60,1120,40), 2);

    g.drawRect     (juce::Rectangle(1130,0,260,150), 2);
    g.drawRect     (juce::Rectangle(1130,160,260,150), 2);
    g.drawRect     (juce::Rectangle(1130,530,260,270), 2);
}

void FxseqAudioProcessorEditor::resized()
{
    int seqHeigth=60;
    int seqsSize=(sizeof(sequencers)/sizeof(sequencers[0])-1)*seqHeigth;
    int sequencerEndY=seqsSize+50;    

    debugLog.setBounds(1130,320,260,200);

    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {
        //sequencers[i].setBounds(5,5+i*80,1200,80);
        sequencers[i].setBounds(5,5+sequencers[i].position*seqHeigth,1200,seqHeigth);
    }

    sequenceSeq.setBounds(5,sequencerEndY,1200,seqHeigth);

    for (int i=0;i<sizeof(effects)/sizeof(effects[0]);i++) {
        if (i<4)
        {effects[i].setBounds(5+i*282,sequencerEndY+65,262,120);}
        else {effects[i].setBounds(5+(i-4)*282,sequencerEndY+65+140,262,120);}
    }

    options.setBounds(1135,5,250,140);

    output.setBounds(1135,165,250,140);

    logo.setBounds(1135,sequencerEndY+65,250,260);
}

void FxseqAudioProcessorEditor::timerCallback()
{    
    double ppq=audioProcessor.ppq;
    float unused;
    std::vector<int> positions;
    int sequencePosition=0;
    int sequenceLength=sequenceSeq.getSequenceLength();
    std::vector<int> clockMults;
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) { clockMults.push_back(sequencers[i].clockMult); }
    greatestClockMult=(int)greatest(clockMults);
    audioProcessor.greatestClockMult=greatestClockMult;

    refreshSequencerPositions();

    if (ppq >= sequenceLength ) {sequencePosition=int(std::floor(ppq/greatestClockMult)-sequenceLength*std::floor(ppq/(sequenceLength*greatestClockMult)));} else {sequenceLength=std::floor(ppq/greatestClockMult);}
    sequenceSeq.updatePosition(sequencePosition);

    std::string pos1,pos2,select;
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) { 
        pos1+=" " + std::to_string(audioProcessor.fxPositions[i]);
        select+=" " + std::to_string(audioProcessor.selected_pattern[i]);

        positions.push_back(int(std::floor( std::modf(ppq/sequencers[i].clockMult,&unused)*(16))));

        if ( (options.scroll) && (options.sequenceMode) ) {
            sequencers[i].remoteChangeSelectedPattern(sequenceSeq.sequence[sequencePosition]);
        }

        sequencers[i].updatePosition(positions[i]);
        
        if ( getFxParamProperty(i,0,0,"hasPrograms")[0] == "0") {  effects[i].hidePrograms();/*programButton.setVisible(false);*/}   
    }
    

    int procSeqLength=audioProcessor.sequenceLength;
    debugLog.setText(  rootPath + "\n"
                       +"ppq " + std::to_string(ppq) + "\n"
                       +"seq length: " + std::to_string(procSeqLength) +"\n"
                       +"seqmode :"+ std::to_string(options.sequenceMode) +" scroll :" + std::to_string(options.scroll) + "\n"
                       +"selected pattern : " + select +  "\n"
                       + "fx chain " +  pos1 + "\n"
                      // + audioProcessor.debug  + "\n" 	
                       + std::to_string(audioProcessor.getParameterValue("CombFilter_Frequency")) +"\n"
                       + std::to_string(audioProcessor.getParameterValue("CombFilter_Feedback")) +"\n"
                      // +std::to_string(getMasterParam("sequencerMode")) + "\n"
                      // +std::to_string(this->getNumChildComponents()) +"\n"
                  //     +std::to_string(effects[1].getNumChildComponents()) +"\n"
                  //     +std::to_string(sequencers[0].getNumChildComponents()) +"\n"
                       //+std::to_string(getMasterParam("Chopper_dry/wet")) +"\n"
                    );
}

void FxseqAudioProcessorEditor::updateSeqPattern(int sequencerIndex,int patternIndex)
{
    sequencers[sequencerIndex].pattern=audioProcessor.patterns[sequencerIndex][patternIndex];
}

void FxseqAudioProcessorEditor::updateProcessorPattern(int sequencerIndex,int patternIndex) 
{
    audioProcessor.patterns[sequencerIndex][patternIndex]=sequencers[sequencerIndex].pattern;
    audioProcessor.updateGainPattern(sequencerIndex,patternIndex);
}

void FxseqAudioProcessorEditor::updateSelectedProcessorEffect(int sequencerIndex,int effectIndex) 
{
    //std::string paramName=fxNamesStr[sequencerIndex] + "_fx";
    //audioProcessor.updateParameter(paramName,(float)effectIndex);
}

std::vector<int> FxseqAudioProcessorEditor::getSequence(int seqIndex)
{
    return audioProcessor.sequences[seqIndex];
}


void FxseqAudioProcessorEditor::updateSequence(int seqIndex, std::vector<int> sequence)
{
    audioProcessor.sequences[seqIndex]=sequence;
}

void FxseqAudioProcessorEditor::changeFxPosition(int seqIndex,int newPosition)
{
    int indexAtNewPosition,replacePosition;
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {  if (sequencers[i].position==newPosition) {indexAtNewPosition=i;}    } 
    replacePosition=sequencers[seqIndex].position;
    sequencers[indexAtNewPosition].position=replacePosition;
    sequencers[seqIndex].position=newPosition;
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) 
    { 
        audioProcessor.updateParameter(fxNamesStr[i]+"_position",(float)sequencers[i].position);
        for (int j=0;j<sizeof(sequencers)/sizeof(sequencers[0]);j++) 
        { 
            if (sequencers[j].position == i)
            {
                audioProcessor.fxPositions[i]=j;
            }
        }

    }
    resized();
    repaint();
}

void FxseqAudioProcessorEditor::refreshSequencerPositions()
{
    bool change=false;
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) 
    {
       if (int(std::find(std::begin(audioProcessor.fxPositions), std::end(audioProcessor.fxPositions), i) - std::begin(audioProcessor.fxPositions)) != sequencers[i].position )
        {
            change=true;
            sequencers[i].position=int(std::find(std::begin(audioProcessor.fxPositions), std::end(audioProcessor.fxPositions), i) - std::begin(audioProcessor.fxPositions));
        }        

    }
    if (change) {
        resized();
        repaint();
    }
}

int FxseqAudioProcessorEditor::getSequencerPosition(int seqIndex)
{
    return audioProcessor.fxPositions[seqIndex];
}

int FxseqAudioProcessorEditor::getSequencerCount()
{
    return audioProcessor.sequencerCount;
}


void FxseqAudioProcessorEditor::updateMaster(std::string parameterName,float value)
{
    audioProcessor.updateParameter(parameterName,value);
}

float FxseqAudioProcessorEditor::getMasterParam(std::string parameterName)
{
    return audioProcessor.getParameterValue(parameterName);
}

void FxseqAudioProcessorEditor::updateFxParam(int fxIndex,int programIndex, float paramIndex,float paramValue)
{
    std::string paramName=effects[fxIndex].name +"_dry/wet";
    std::string processorParamName=effects[fxIndex].name +"_";
    if (effects[fxIndex].name == "Echo") {
        /*if (paramIndex==0) {
            audioProcessor.echo_setDelay(paramValue);
        }
        if (paramIndex==1) {
            audioProcessor.echo_setFeedback(paramValue);
        }*/
        if (paramIndex==0) { processorParamName  += "Time";audioProcessor.updateParameter(processorParamName,paramValue);}
        //std::string paramName=effects[fxIndex].name + "_" + effects[fxIndex].paramsLabel[fxIndex].getText().toStdString();
        if (paramIndex==1) { processorParamName  += "Feedback";audioProcessor.updateParameter(processorParamName,paramValue); } 
    }

    if (effects[fxIndex].name == "Crusher") {
        if (paramIndex==0) {
            audioProcessor.updateEffectProgramParameter(fxIndex,programIndex,paramIndex,paramValue); 
        }
        if (paramIndex==1) {
            audioProcessor.updateEffectProgramParameter(fxIndex,programIndex,paramIndex,paramValue); 
        }
    }

    if (effects[fxIndex].name == "Filter") {
        if (paramIndex==0) {
            audioProcessor.updateEffectProgramParameter(fxIndex,programIndex,paramIndex,(float)paramValue); 
        }
        if (paramIndex==1) {
            audioProcessor.updateEffectProgramParameter(fxIndex,programIndex,paramIndex,paramValue); 
        }
        if (paramIndex==2) {
            audioProcessor.updateEffectProgramParameter(fxIndex,programIndex,paramIndex,paramValue); 
        }
    }

    if (effects[fxIndex].name == "Distortion") {
        if (paramIndex==0) {
            audioProcessor.updateParameter(effects[fxIndex].name + "_Type",paramValue);
       }
    }

     if (effects[fxIndex].name == "Repeater") {
        if (paramIndex==0) { processorParamName  += "size"; audioProcessor.updateParameter(processorParamName,paramValue);} 
       /* if (paramIndex==0) { processorParamName  += "mSize";audioProcessor.updateParameter(processorParamName,paramValue);}
        //std::string paramName=effects[fxIndex].name + "_" + effects[fxIndex].paramsLabel[fxIndex].getText().toStdString();
        if (paramIndex==1) { processorParamName  += "count";audioProcessor.updateParameter(processorParamName,paramValue); } 
        if (paramIndex==1) { processorParamName  += "size"; audioProcessor.repeater.changeParameter(paramIndex,paramValue);} */
        //audioProcessor.updateParameter(processorParamName,paramValue);
    }
    if (effects[fxIndex].name == "RingMod") {
        if (paramIndex==0) {
            //audioProcessor.ringMod_setFreq(paramValue);
            processorParamName  += "Frequency";audioProcessor.updateParameter(processorParamName,paramValue);
       }
        if (paramIndex==1) {
            //audioProcessor.ringMod_setDepth(paramValue);
            processorParamName  += "ModDepth";audioProcessor.updateParameter(processorParamName,paramValue);
       }
    }

    if (effects[fxIndex].name == "CombFilter") {
        if (paramIndex==0) {
            //audioProcessor.combFilter_setFrequency(paramValue);
            processorParamName  += "Frequency";audioProcessor.updateParameter(processorParamName,paramValue);
       }
        if (paramIndex==1) {
            //audioProcessor.combFilter_setFeedback(paramValue);
            processorParamName  += "Feedback";audioProcessor.updateParameter(processorParamName,paramValue);
       }
    }

     if (effects[fxIndex].name == "PitchShifter") {
        if (paramIndex==0) {
            //audioProcessor.pitchShifter_setPitch(paramValue);
            audioProcessor.updateParameter("PitchShifter_Frequency",paramValue);
        }
        if (paramIndex==1) {
            //audioProcessor.pitchShifter_setWindow(paramValue);
            audioProcessor.updateParameter("PitchShifter_Window",paramValue);
        }
        if (paramIndex==2) {
            //audioProcessor.pitchShifter_setXfade(paramValue);
            audioProcessor.updateParameter("PitchShifter_Xfade",paramValue);
        }
    }
}


std::vector<std::string> FxseqAudioProcessorEditor::getFxParamProperty(int fxIndex, int paramIndex, int programIndex,std::string paramProperty)
{
    audioProcessor.getParameterProperty(fxIndex,paramIndex,paramProperty,programIndex);
    
    if (effects[fxIndex].name == "Echo") {
        if (paramProperty == "hasPrograms") { return {"0"}; }

        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Time"}; }
            if (paramProperty == "range") {return {"0.00","1.00","0.10"};}
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("Echo_Time")/*audioProcessor.echo_time*/)};}       
        }
        if (paramIndex==1) {
            if (paramProperty == "name")  {return {"Feedback"}; }
            if (paramProperty == "range") {return {"0.00","1.00","0.10"};}
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("Echo_Feedback")/*audioProcessor.echo_feedback*/)};}
        }
    }

    if (effects[fxIndex].name == "Crusher") {
        if (paramProperty == "hasPrograms") { return {"1"}; }

        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Reduction"}; }
            if (paramProperty == "range") {return {"0.00","30.00","0.10"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
        if (paramIndex==1) {
            if (paramProperty == "name")  {return {"Bitdepth"}; }
            if (paramProperty == "range") {return {"1.00","32.00","1.0"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
    }

    if (effects[fxIndex].name == "Filter") {
        if (paramProperty == "hasPrograms") { return {"1"}; }

        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Frequency"}; }
            if (paramProperty == "range") {return {"20.00","20000.00","1.0"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
        if (paramIndex==1) {
            if (paramProperty == "name")  {return {"Resonance"}; }
            if (paramProperty == "range") {return {"0.00","1.00","0.1"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
        if (paramIndex==2) {
            if (paramProperty == "name")  {return {"Drive"}; }
            if (paramProperty == "range") {return {"1.00","10.00","0.1"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
    
    }

    if (effects[fxIndex].name == "Distortion") {
        if (paramProperty == "hasPrograms") { return {"0"}; }
        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Type"}; }
            if (paramProperty == "range") {return {"0.0","1.0","1.0"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("Distortion_Type"))};} 
        }
        
    }

    if (effects[fxIndex].name == "Repeater") {
        //audioProcessor.getParameterProperty(fxIndex,paramIndex , paramProperty, programIndex);
        if (paramProperty == "hasPrograms") { return {"0"}; }
        /*if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Length"}; }
            if (paramProperty == "range") {return {"1.0","4.0","1.0"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }*/
        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"size"}; }
            if (paramProperty == "range") {return {"1.0","4.0","1.0"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("Repeater_size"))};} 
        }
        /*
        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"mSize"}; }
            if (paramProperty == "range") {return {"200.0","2000.0","0.1"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("Repeater_mSize"))};} 
        }
        if (paramIndex==1) {
            if (paramProperty == "name")  {return {"count"}; }
            if (paramProperty == "range") {return {"1.0","16.0","1.0"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("Repeater_count"))};}
            //audioProcessor.getParameterValue(effects[fxIndex].name + "_" + effects[fxIndex].paramsLabel[fxIndex].getText().toStdString()) 
        }
        if (paramIndex==2) {
            if (paramProperty == "name")  {return {"size"}; }
            if (paramProperty == "range") {return {"2.0","200.0","0.1"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("Repeater_size"))};}
            //audioProcessor.getParameterValue(effects[fxIndex].name + "_" + effects[fxIndex].paramsLabel[fxIndex].getText().toStdString())
        }*/
    }

    if (effects[fxIndex].name == "RingMod") {
        if (paramProperty == "hasPrograms") { return {"0"}; }
        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Frequency"}; }
            if (paramProperty == "range") {return {"1.0","5000.0","0.1"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("RingMod_Frequency"))};} 
       }
        if (paramIndex==1) {
            if (paramProperty == "name")  {return {"ModDepth"}; }
            if (paramProperty == "range") {return {"0.0","1.0","0.1"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("RingMod_ModDepth"))};} 
       }
    }

   if (effects[fxIndex].name == "CombFilter") {
        if (paramProperty == "hasPrograms") { return {"0"}; }
        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Frequency"}; }
            if (paramProperty == "range") {return {"0.0","1000.0","0.1"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("CombFilter_Frequency"))};} 
       }
        if (paramIndex==1) {    
            if (paramProperty == "name")  {return {"Feedback"}; }
            if (paramProperty == "range") {return {"0.0","0.99","0.01"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("CombFilter_Feedback"))};} 
       }
    }

    if (effects[fxIndex].name == "PitchShifter") {
        if (paramProperty == "hasPrograms") { return {"0"}; }
        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Pitch"}; }
            if (paramProperty == "range") {return {"-12.0","12.0","0.1"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("PitchShifter_Frequency"))};} 
       }
        if (paramIndex==1) {    
            if (paramProperty == "name")  {return {"Window"}; }
            if (paramProperty == "range") {return {"50.0","10000.0","1.00"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("PitchShifter_Window"))};} 
       }
        if (paramIndex==2) {    
            if (paramProperty == "name")  {return {"Xfade"}; }
            if (paramProperty == "range") {return {"1.0","10000.0","1.00"}; }
            if (paramProperty == "value") {return {std::to_string(audioProcessor.getParameterValue("PitchShifter_Xfade"))};} 
       }
    }

    if (paramProperty == "hasPrograms") { return {"0"}; } 
    if (paramProperty == "name") {return {""};}
    if (paramProperty == "range") {return {"0.00","1.00","0.1"}; }
    if (paramProperty == "value") {return {"-100"};}

    return {};
}

void FxseqAudioProcessorEditor::updateAllFxParamValues()
{
    audioProcessor.debug="";
    for(int i=0;i<sizeof(effects)/sizeof(effects[0]);i++)
    {
        for (int j=0;j<3;j++) 
        {
            std::string value=getFxParamProperty(i,j,0,"value")[0];
            if ( value != "-100")
            {
                effects[i].params[j].setValue(std::stof(value));
                audioProcessor.debug+= effects[i].name + "_" + getFxParamProperty(i,j,0,"name")[0] + " = " + value+ "\n";
            }
        }   
       
    }
}

void FxseqAudioProcessorEditor::patternUtils(std::string action,int seqIndex)
{
    int index;
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++)
    {
        if (sequencers[i].position == seqIndex) {index=i;}
    }

    if (action == "copy")
    {
        clipboard=sequencers[index].pattern;
    }
    if (action == "paste")
    {
        sequencers[index].pattern=clipboard;
        updateProcessorPattern(index,sequencers[index].getSelectedPattern());
    }
    if (action == "clear")
    {
        sequencers[index].pattern={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        updateProcessorPattern(seqIndex,sequencers[index].getSelectedPattern());
    }
    if (action == "reset")
    {
        for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++)
        {
            sequencers[i].pattern={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            updateProcessorPattern(i,sequencers[i].getSelectedPattern());
        }
    }
    
}

void FxseqAudioProcessorEditor::loadPreset(std::string presetName)
{
    audioProcessor.loadPatternsAndSequencesFromXMLFile(getPath("presets")+presetName + ".xml");
    //debugLog.setText(audioProcessor.debug);
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++)
    {
        updateSeqPattern(i,sequencers[i].getSelectedPattern());
    }
    
    refreshSequencerPositions();
    updateAllFxParamValues();
    
}
////////////////////////////////////////////////////////////////////////////////////// UTILS //////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> FxseqAudioProcessorEditor::split(std::string s, std::string delimiter) 
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

int FxseqAudioProcessorEditor::greatest(int n1,int n2,int n3,int n4) 
{
    int ng1,ng2,Greatest;
    if(n1>=n2) { ng1=n1; } else {ng1=n2;}
    if(n3>=n4) { ng2=n3; } else {ng2=n4;}
    if (ng1>=ng2) { Greatest=ng1;} else { Greatest=ng2;}

    return Greatest;
}

int FxseqAudioProcessorEditor::greatest(std::vector<int> seqClockDivs)
{
    int v=0;
    for (int clock : seqClockDivs )
    {
        if (clock>v)
        {
            v=clock;
        }
    }
    return v;
} 

int FxseqAudioProcessorEditor::lowest(int n1,int n2,int n3,int n4) 
{
    int ng1,ng2,Lowest;
    if(n1>=n2) { ng1=n2; } else {ng1=n1;}
    if(n3>=n4) { ng2=n4; } else {ng2=n3;}
    if (ng1>=ng2) { Lowest=ng2;} else { Lowest=ng1;}

    return Lowest;
}
////////////////////////////////////////////////////////////////////////////////////// PATHS ////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> FxseqAudioProcessorEditor::getFileSeparators()
{
    // /!\ PLATFORM SPECIFIC CODE
    std::vector<std::string> result;
    std::string sep,hidden;
    std::string operatingSystem=juce::SystemStats::getOperatingSystemName().toStdString();
    // WINDOWS
    if ( operatingSystem.find("Windows") !=std::string::npos )
    {
        sep+=(char) 92;
        hidden="";
    }
    // LINUX / APPLE / ANDROID
    if ( ( operatingSystem.find("Linux") !=std::string::npos ) or ( operatingSystem.find("Mac") !=std::string::npos ) )
    {
        sep="/";
        hidden=".";
    }
    
    result.push_back(sep);
    result.push_back(hidden);

    return result;
}

void FxseqAudioProcessorEditor::initDirectories()
{   
    std::vector<std::string> separators=getFileSeparators();
    std::string sep=separators[0],hidden=separators[1];

    
    juce::File homeDir = juce::File(juce::File::getSpecialLocation (juce::File::userHomeDirectory));
    const std::string homePath=homeDir.getFullPathName().toStdString();
    std::string devPath=homePath+ sep + hidden + "ssabug" +sep;
    rootPath=devPath+"fxseq"+ sep;
    
    if ( !std::filesystem::exists(devPath) )  {
        std::filesystem::create_directory(devPath);
    }

    if ( !std::filesystem::exists(rootPath) )  {
        std::filesystem::create_directory(rootPath);
        std::filesystem::create_directory(getPath("config"));
        std::filesystem::create_directory(getPath("presets"));
        std::filesystem::create_directory(getPath("skins"));

        std::filesystem::create_directory(getPath("currentSkin"));
        std::filesystem::create_directory(getPath("images"));
    }
   
}


std::string FxseqAudioProcessorEditor::getPath(std::string path)
{
    initDirectories();

    std::vector<std::string> separators=getFileSeparators();
    std::string sep=separators[0],hidden=separators[1]; 

    if (path ==  "root") {
        return rootPath;
    }
    if (path ==  "config") {
        return rootPath + "config" + sep ;
    }
    if (path ==  "configFile") {
        return rootPath + "config"+ sep + "config.xml";
    }
    if (path ==  "skins") {
        return rootPath + "skins" + sep;
    }
    if (path ==  "currentSkin") {
        return rootPath + "skins" + sep + currentSkin + sep;
    }
    if (path ==  "currentSkinFile") {
        return rootPath + "skins" + sep + currentSkin + sep + "skin.xml";
    }
    if (path ==  "images") {
        return rootPath + "skins" + sep + currentSkin + sep + "images" + sep;
    }
    if (path ==  "presets") {
        return rootPath + "presets" + sep;
    }
    if (path ==  "currentPreset") {
        return rootPath + "presets" + sep + currentPreset +".xml";
    }

    return "";
}

std::vector<std::string> FxseqAudioProcessorEditor::get_directories(const std::string& s)
{
    std::vector<std::string> r;
    for(auto& p : std::filesystem::recursive_directory_iterator(s))
        if (p.is_directory())
            r.push_back(p.path().string());
    return r;
}

std::vector<std::string> FxseqAudioProcessorEditor::getPresetList()
{
    std::vector<std::string> presetFiles;
    for (const auto & entry : std::filesystem::directory_iterator(getPath("presets"))) {
        std::string file=entry.path().string();
        if (file.find(".xml") != std::string::npos ) {
            presetFiles.push_back(file);
        }
    }
    return presetFiles;
}

std::vector<std::string> FxseqAudioProcessorEditor::getSkinList()
{
    std::vector<std::string> separators=getFileSeparators();
    std::string sep=separators[0],hidden=separators[1]; 

    auto folders=get_directories(getPath("skins"));//get available folders in /skins
    std::vector<std::string> skinList;//={"default","red","green","yellow","purple","turquoise"};
    for (auto folder : folders) {      
        if ( std::filesystem::exists(folder+ sep + "skin.xml") ) {            // look for skin file
            std::string folderShort=folder.substr(getPath("skins").length()); // get skin folder name
            //debug.setText(debug.getText()+"\nfound skin "+folderShort);
            skinList.push_back(folderShort);  // add to skin list
        }        
    }
    return skinList;
}

std::string FxseqAudioProcessorEditor::removeForbiddenCharacters(std::string text)
{
    std::vector<std::string> forbiddenCharacters={".",",","/",":","*","?","<",">","|",};
    std::string character,result=text;   
    character=(char) 34;forbiddenCharacters.push_back(character);
    character=(char) 92;forbiddenCharacters.push_back(character);

    for (auto c:forbiddenCharacters) {
        while (text.find(c) != std::string::npos) {         
            text.erase(text.find(c), c.length());
            result=text;
        }
    }
    return result;
}

bool FxseqAudioProcessorEditor::presetExists(std::string presetText)
{
    /*std::vector<std::string> presetList=getPresetList();
    for (auto p:presetList) {
        if( readXMLParam(p,"preset/options/name").find(presetText)  != std::string::npos) {   return true;     }
    }*/
    if ( std::filesystem::exists(std::string(getPath("presets")+presetText+".xml")) )
    {
        return true;
    }

    return false;
}
////////////////////////////////////////////////////////////////////////////////////// XML //////////////////////////////////////////////////////////////////////////////////////
void FxseqAudioProcessorEditor::saveXMLPreset(std::string presetName)
{
    auto state = audioProcessor.pluginParameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());

    juce::XmlElement* patternDef=new juce::XmlElement(*(audioProcessor.getAllPatternsXml()));
    juce::XmlElement* sequenceDef=new juce::XmlElement(*(audioProcessor.getAllSequencesXml()));
    juce::XmlElement* programsDef=new juce::XmlElement(*(audioProcessor.getProgramParametersXml()));

    xml->removeChildElement(xml->getChildByName("PATTERNS"),true);
    xml->removeChildElement(xml->getChildByName("SEQUENCES"),true);
    xml->removeChildElement(xml->getChildByName("PROGRAMS"),true);
    
    xml->addChildElement(patternDef);  
    xml->addChildElement(sequenceDef);
    xml->addChildElement(programsDef);

    std::ofstream file(getPath("presets")+presetName+".xml");
    
    file<<(*xml).toString();
}

std::string FxseqAudioProcessorEditor::readXMLParam(std::string xmlFilePath,std::string paramPath)
{
    std::string rootTagName,categoryTagName,paramTagName,delimiter="/";
    std::vector<std::string> v = split (paramPath, delimiter);      
    rootTagName=v[0];
    categoryTagName=v[1];
    paramTagName=v[2];
    
    if ( !std::filesystem::exists(std::string(xmlFilePath)) )  {

    } else {
        juce::File xmlFile(xmlFilePath);
        juce::XmlDocument xmlDoc(xmlFile);

        if (juce::XmlDocument::parse(xmlFile))
        {
            // Access the root element of the XML document
            auto rootElement = xmlDoc.getDocumentElement();
            if (rootElement->hasTagName(rootTagName)) {
                for (auto* e : rootElement->getChildByName(categoryTagName)->getChildIterator())
                    {
                        juce::String paramName = e->getAllSubText();                     
                        if (e->getTagName().toStdString() == paramTagName) { return e->getAllSubText().toStdString();}
                    }                       
			} 
        }
    }
    return "";
}

std::vector<std::string> FxseqAudioProcessorEditor::readXMLVectorParam(std::string xmlFilePath,std::string paramPath)
{   // skin/templates/imagebutton1/images
    std::string rootTagName,categoryTagName,subCategoryTagName,subSubCategoryTagName,delimiter="/";
    std::vector<std::string> v = split (paramPath, delimiter);
    std::vector<std::string> out;    
    
    rootTagName=v[0];
    categoryTagName=v[1];
    subCategoryTagName=v[2];
    subSubCategoryTagName=v[3];
    
    if ( !std::filesystem::exists(std::string(xmlFilePath)) )  {

    } else {
        juce::File xmlFile(xmlFilePath);
        juce::XmlDocument xmlDoc(xmlFile);

        if (juce::XmlDocument::parse(xmlFile))
        {
            // Access the root element of the XML document
            auto rootElement = xmlDoc.getDocumentElement();
            if (rootElement->hasTagName(rootTagName)) {
                for (auto* e : rootElement->getChildByName(categoryTagName)->getChildIterator()) //->getChildByName(subCategoryTagName)->getChildByName(subSubCategoryTagName)
                    {
                        if (e->getTagName().toStdString() == subCategoryTagName) 
                        {                           
                            for (auto* f:e->getChildByName(subSubCategoryTagName)->getChildIterator())
                            {
                                std::string paramValue = f->getAllSubText().toStdString();                     
                                if (f->getTagName().toStdString() == "entry") { out.push_back( paramValue );}
                            }
                            
                        }
                        
                    }                       
			} 
        }
    }
    return out;
}

std::vector<std::vector<int>> FxseqAudioProcessorEditor::readXMLVector2Param(std::string xmlFilePath,std::string paramPath)
{   // skin/templates/imagebutton1/images
    std::string rootTagName,categoryTagName,subCategoryTagName,subSubCategoryTagName,delimiter="/";
    std::vector<std::string> v = split (paramPath, delimiter);
    std::vector<std::vector<int>> out;    
    
    rootTagName=v[0];
    categoryTagName=v[1];
    subCategoryTagName=v[2];
    subSubCategoryTagName=v[3];
    
    //debugLog.setText(debugLog.getText() + "\n" + xmlFilePath + "\n" + rootTagName + "\n" + categoryTagName + "\n" + subCategoryTagName + "\n" + subSubCategoryTagName);

    if ( !std::filesystem::exists(std::string(xmlFilePath)) )  {

    } else {
        juce::File xmlFile(xmlFilePath);
        juce::XmlDocument xmlDoc(xmlFile);

        if (juce::XmlDocument::parse(xmlFile))
        {
            // Access the root element of the XML document
            auto rootElement = xmlDoc.getDocumentElement();
            if (rootElement->hasTagName(rootTagName)) {
                for (auto* e : rootElement->getChildByName(categoryTagName)->getChildIterator()) //->getChildByName(subCategoryTagName)->getChildByName(subSubCategoryTagName)
                    {
                        if (e->getTagName().toStdString() == subCategoryTagName) 
                        {                           
                            for (auto* f:e->getChildByName(subSubCategoryTagName)->getChildIterator())
                            {
                                std::string colorIdString =    f->getChildByName("id")->getAllSubText().toStdString();
                                std::string colorValueString = f->getChildByName("color")->getAllSubText().toStdString();              
                                if (f->getTagName().toStdString() == "entry") { out.push_back( {(int) std::stoi(colorIdString,nullptr,16) , (int)std::stoul(colorValueString,nullptr,16) } );}
                            }
                            
                        }
                        
                    }                       
			} 
        }
    }
    return out;
}

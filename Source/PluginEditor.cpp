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
    setSize (1400, 500);

    //initDirectories();

    std::string skinFilePath=getPath("currentSkinFile");

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

    startTimerHz(30);
}

FxseqAudioProcessorEditor::~FxseqAudioProcessorEditor()
{
}

//==============================================================================
void FxseqAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll (juce::Colours::black);
    
    g.setFont (15.0f);
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {
        g.setColour (juce::Colour(sequencers[i].stepSeqColors[1][1]));
        g.drawRect (juce::Rectangle(0,sequencers[i].position*80,140,70), 2);
        g.drawRect (juce::Rectangle(0,sequencers[i].position*80,1120,70), 2);
        g.drawRect (juce::Rectangle(i*282,370,272,130), 2);
    }
    g.setColour (juce::Colour(0xFF3F3F7F));
    g.drawRect     (juce::Rectangle(0,320,140,40), 2);
    g.drawRect     (juce::Rectangle(0,320,1120,40), 2);

    g.drawRect     (juce::Rectangle(1130,0,260,150), 2);
    g.drawRect     (juce::Rectangle(1130,160,260,150), 2);
}

void FxseqAudioProcessorEditor::resized()
{
    
    debugLog.setBounds(1130,320,260,180);

    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {
        //sequencers[i].setBounds(5,5+i*80,1200,80);
        sequencers[i].setBounds(5,5+sequencers[i].position*80,1200,80);
    }

    sequenceSeq.setBounds(5,310,1200,80);

    for (int i=0;i<sizeof(effects)/sizeof(effects[0]);i++) {
        effects[i].setBounds(5+i*282,375,262,120);
    }

    options.setBounds(1135,5,250,140);

    output.setBounds(1135,165,250,140);
}

void FxseqAudioProcessorEditor::timerCallback()
{    
    double ppq=audioProcessor.ppq;
    float unused;
    std::vector<int> positions;
    int sequencePosition;
    int sequenceLength=sequenceSeq.getSequenceLength();
    greatestClockMult=(int)greatest(sequencers[0].clockMult,sequencers[1].clockMult,sequencers[2].clockMult,sequencers[3].clockMult);
    audioProcessor.greatestClockMult=greatestClockMult;

    refreshSequencerPositions();

    if (ppq >= sequenceLength ) {sequencePosition=int(std::floor(ppq/greatestClockMult)-sequenceLength*std::floor(ppq/(sequenceLength*greatestClockMult)));} else {sequenceLength=std::floor(ppq/greatestClockMult);}
    sequenceSeq.updatePosition(sequencePosition);

    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {       
        positions.push_back(int(std::floor( std::modf(ppq/sequencers[i].clockMult,&unused)*(16))));

        if ( (options.scroll) and (options.sequenceMode) ) {
            sequencers[i].remoteChangeSelectedPattern(sequenceSeq.sequence[sequencePosition]);
        }

        sequencers[i].updatePosition(positions[i]);
        
        if ( getFxParamProperty(i,0,0,"hasPrograms")[0] == "0") {  effects[i].hidePrograms();/*programButton.setVisible(false);*/}   
    }
    
    int procSeqLength=audioProcessor.sequenceLength;
    debugLog.setText(    "ppq " + std::__cxx11::to_string(ppq) + "\n"
                       +"selected pattern : " + std::__cxx11::to_string(audioProcessor.selected_pattern[0])+" " + std::__cxx11::to_string(audioProcessor.selected_pattern[1])+ " " + std::__cxx11::to_string(audioProcessor.selected_pattern[2]) + " " + std::__cxx11::to_string(audioProcessor.selected_pattern[3]) + "\n"
                       +"seq length: " + std::__cxx11::to_string(procSeqLength) +"\n"
                       +"seqmode :"+ std::__cxx11::to_string(options.sequenceMode) +" scroll :" + std::__cxx11::to_string(options.scroll) + "\n"
                       +std::__cxx11::to_string(audioProcessor.getParameterValue(fxNamesStr[0]+"_position")) +"\n"
                       +std::__cxx11::to_string(getMasterParam("sequencerMode"))
                      // +std::__cxx11::to_string(this->getNumChildComponents()) +"\n"
                  //     +std::__cxx11::to_string(effects[1].getNumChildComponents()) +"\n"
                  //     +std::__cxx11::to_string(sequencers[0].getNumChildComponents()) +"\n"
                       //+std::__cxx11::to_string(getMasterParam("Chopper_dry/wet")) +"\n"
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
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) { 
        audioProcessor.fxPositions[i]=sequencers[i].position;
        audioProcessor.updateParameter(fxNamesStr[i]+"_position",(float)sequencers[i].position); 
    }
    resized();
    repaint();
}

void FxseqAudioProcessorEditor::refreshSequencerPositions()
{
    bool change=false;
    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) { 
        if (sequencers[i].position != audioProcessor.fxPositions[i]) {
            sequencers[i].position=audioProcessor.fxPositions[i];
            change=true;
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
    return (int)(sizeof(sequencers)/sizeof(sequencers[0]));
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
    if (effects[fxIndex].name == "Echo") {
        if (paramIndex==0) {
            audioProcessor.echo_setDelay(paramValue);
        }
        if (paramIndex==1) {
            audioProcessor.echo_setFeedback(paramValue);
        }
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


}

std::vector<std::string> FxseqAudioProcessorEditor::getFxParamProperty(int fxIndex, int paramIndex, int programIndex,std::string paramProperty)
{
    
    if (effects[fxIndex].name == "Echo") {
        if (paramProperty == "hasPrograms") { return {"0"}; }

        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Time"}; }
            if (paramProperty == "range") {return {"0.00","1.00","0.10"};}
            if (paramProperty == "value") {return {std::__cxx11::to_string(audioProcessor.echo_time)};}       
        }
        if (paramIndex==1) {
            if (paramProperty == "name")  {return {"Feedback"}; }
            if (paramProperty == "range") {return {"0.00","1.00","0.10"};}
            if (paramProperty == "value") {return {std::__cxx11::to_string(audioProcessor.echo_feedback)};}
        }
    }

    if (effects[fxIndex].name == "Crusher") {
        if (paramProperty == "hasPrograms") { return {"1"}; }

        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Reduction"}; }
            if (paramProperty == "range") {return {"0.00","30.00","0.10"}; }
            if (paramProperty == "value") {return {std::__cxx11::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
        if (paramIndex==1) {
            if (paramProperty == "name")  {return {"Bitdepth"}; }
            if (paramProperty == "range") {return {"1.00","32.00","1.0"}; }
            if (paramProperty == "value") {return {std::__cxx11::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
    }

    if (effects[fxIndex].name == "Filter") {
        if (paramProperty == "hasPrograms") { return {"1"}; }

        if (paramIndex==0) {
            if (paramProperty == "name")  {return {"Frequency"}; }
            if (paramProperty == "range") {return {"20.00","20000.00","10.0"}; }
            if (paramProperty == "value") {return {std::__cxx11::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
        if (paramIndex==1) {
            if (paramProperty == "name")  {return {"Resonance"}; }
            if (paramProperty == "range") {return {"0.00","1.00","0.1"}; }
            if (paramProperty == "value") {return {std::__cxx11::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
        if (paramIndex==2) {
            if (paramProperty == "name")  {return {"Drive"}; }
            if (paramProperty == "range") {return {"1.00","10.00","0.1"}; }
            if (paramProperty == "value") {return {std::__cxx11::to_string(audioProcessor.fxPrograms[fxIndex][programIndex+1][paramIndex])};} 
        }
    
    }
   
    if (paramProperty == "hasPrograms") { return {"0"}; } 
    if (paramProperty == "name") {return {""};}
    if (paramProperty == "range") {return {"0.00","1.00","0.1"}; }
    if (paramProperty == "value") {return {"-100"};}

    return {};
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

int FxseqAudioProcessorEditor::lowest(int n1,int n2,int n3,int n4) 
{
    int ng1,ng2,Lowest;
    if(n1>=n2) { ng1=n2; } else {ng1=n1;}
    if(n3>=n4) { ng2=n4; } else {ng2=n3;}
    if (ng1>=ng2) { Lowest=ng2;} else { Lowest=ng1;}

    return Lowest;
}
////////////////////////////////////////////////////////////////////////////////////// PATHS ////////////////////////////////////////////////////////////////////////////////////
void FxseqAudioProcessorEditor::initDirectories()
{
    // /!\ PLATFORM SPECIFIC CODE
    juce::File homeDir = juce::File(juce::File::getSpecialLocation (juce::File::userHomeDirectory));
    const std::string homePath=homeDir.getFullPathName().toStdString();
    std::string devPath=homePath+"/.ssabug/";
    rootPath=devPath+"fxseq/";
    
    if ( not std::filesystem::exists(devPath) )  {
        std::filesystem::create_directory(devPath);
    }

    if ( not std::filesystem::exists(rootPath) )  {
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
    if (path ==  "root") {
        return rootPath;
    }
    if (path ==  "config") {
        return rootPath + "config/";
    }
    if (path ==  "configFile") {
        return rootPath + "config/config.xml";
    }
    if (path ==  "skins") {
        return rootPath + "skins/";
    }
    if (path ==  "currentSkin") {
        return rootPath + "skins/" + currentSkin + "/";
    }
    if (path ==  "currentSkinFile") {
        return rootPath + "skins/" + currentSkin + "/skin.xml";
    }
    if (path ==  "images") {
        return rootPath + "skins/" + currentSkin + "/images/";
    }
    if (path ==  "presets") {
        return rootPath + "presets/";
    }
    if (path ==  "currentPreset") {
        return rootPath + "presets/" + currentPreset +".xml";
    }

    return "";
}

////////////////////////////////////////////////////////////////////////////////////// XML //////////////////////////////////////////////////////////////////////////////////////
void FxseqAudioProcessorEditor::saveXMLPreset()
{
    //audioProcessor.getStateInformation();
}

std::string FxseqAudioProcessorEditor::readXMLParam(std::string xmlFilePath,std::string paramPath)
{
    std::string rootTagName,categoryTagName,paramTagName,delimiter="/";
    std::vector<std::string> v = split (paramPath, delimiter);      
    rootTagName=v[0];
    categoryTagName=v[1];
    paramTagName=v[2];
    
    if ( not std::filesystem::exists(std::string(xmlFilePath)) )  {

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
    
    if ( not std::filesystem::exists(std::string(xmlFilePath)) )  {

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

    if ( not std::filesystem::exists(std::string(xmlFilePath)) )  {

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

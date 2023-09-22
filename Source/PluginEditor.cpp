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

    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {
        std::vector<std::string> images=readXMLVectorParam(skinPath+"skin.xml","skin/templates/"+sequencers[i].stepSeqTemplate+"/images");
        sequencers[i].stepSeqColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+sequencers[i].stepSeqTemplate+"/colors");
        sequencers[i].comboColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+sequencers[i].comboTemplate+"/colors");

        sequencers[i].stepSeqImages.clear();
        for (int j=0;j<images.size();j++) {   sequencers[i].stepSeqImages.push_back(juce::ImageFileFormat::loadFrom(juce::File(imagePath+images[j])));    }
        sequencers[i].stepMaxValue=sequencers[i].stepSeqImages.size()-2; 
        updateSeqPattern(i,sequencers[i].getSelectedPattern());
        sequencers[i].skinChange();
        addAndMakeVisible(sequencers[i]);
    }
  
    for (int i=0;i<sizeof(effects)/sizeof(effects[0]);i++) {
        effects[i].comboColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+effects[i].comboTemplate+"/colors");
        effects[i].sliderColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+effects[i].sliderTemplate+"/colors");
        effects[i].programButtonColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+effects[i].imageButtonTemplate+"/colors");
        std::vector<std::string> imagesP=readXMLVectorParam(skinPath+"skin.xml","skin/templates/"+effects[i].imageButtonTemplate+"/images");
        effects[i].programButtonImages.clear();
        for (int j=0;j<imagesP.size();j++) {   effects[i].programButtonImages.push_back(juce::ImageFileFormat::loadFrom(juce::File(imagePath+imagesP[j])));    }
        
        effects[i].skinChange();
        addAndMakeVisible(effects[i]);
    }

    options.comboColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+options.comboTemplate+"/colors");
    options.sliderColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+options.sliderTemplate+"/colors");
    options.imageButtonColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+options.imageButtonTemplate+"/colors");
    options.skinChange();
    addAndMakeVisible(options);

    output.comboColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+output.comboTemplate+"/colors");
    output.sliderColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+output.sliderTemplate+"/colors");
    output.imageButtonColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+output.imageButtonTemplate+"/colors");
    output.skinChange();
    addAndMakeVisible(output);

    sequenceSeq.comboColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+output.comboTemplate+"/colors");
    sequenceSeq.sliderColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+output.sliderTemplate+"/colors");
    sequenceSeq.imageButtonColors=readXMLVector2Param(skinPath+"skin.xml","skin/templates/"+output.imageButtonTemplate+"/colors");
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
        g.drawRect (juce::Rectangle(0,i*80,140,70), 2);
        g.drawRect (juce::Rectangle(0,i*80,1120,70), 2);
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
        sequencers[i].setBounds(5,5+i*80,1200,80);
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

    for (int i=0;i<sizeof(sequencers)/sizeof(sequencers[0]);i++) {       
        positions.push_back(int(std::floor( std::modf(ppq/sequencers[i].clockMult,&unused)*(16))));
        sequencers[i].updatePosition(positions[i]);
    }

    debugLog.setText(    "ppq " + std::__cxx11::to_string(ppq) + "\n"
                       +"position1 " + std::__cxx11::to_string(positions[0]) + "\n"
                       +"position2 " + std::__cxx11::to_string(positions[1]) + "\n"
                       +"position3 " + std::__cxx11::to_string(positions[2]) + "\n"
                       +"position4 " + std::__cxx11::to_string(positions[3]) + "\n"

                    );
 
}

void FxseqAudioProcessorEditor::updateSeqPattern(int sequencerIndex,int patternIndex)
{
    sequencers[sequencerIndex].pattern=audioProcessor.patterns[sequencerIndex][patternIndex];
}

void FxseqAudioProcessorEditor::updateProcessorPattern(int sequencerIndex,int patternIndex) 
{
    audioProcessor.patterns[sequencerIndex][patternIndex]=sequencers[sequencerIndex].pattern;
    //  audioProcessor.updateGainPattern(sequencerIndex,patternIndex);
    //audioProcessor.updateGainPattern( sequencerIndex,patternIndex );
}

void FxseqAudioProcessorEditor::updateSelectedProcessorPattern(int sequencerIndex,int patternIndex) 
{
    std::string paramName="seq"+std::__cxx11::to_string(sequencerIndex+1)+"_pattern";
    audioProcessor.updateParameter(paramName,(float)patternIndex);
    //  audioProcessor.updateGainPattern(sequencerIndex,patternIndex);
}

void FxseqAudioProcessorEditor::updateSelectedProcessorClock(int sequencerIndex,int clockIndex) 
{
    std::string paramName="seq"+std::__cxx11::to_string(sequencerIndex+1)+"_clockDiv";
    audioProcessor.updateParameter(paramName,(float)clockIndex);
}

void FxseqAudioProcessorEditor::updateSelectedProcessorEffect(int sequencerIndex,int effectIndex) 
{
    std::string paramName="seq"+std::__cxx11::to_string(sequencerIndex+1)+"_fx";
    audioProcessor.updateParameter(paramName,(float)effectIndex);
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
////////////////////////////////////////////////////////////////////////////////////// XML //////////////////////////////////////////////////////////////////////////////////////
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

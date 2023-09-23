#include "SequenceSequencerComponent.h"
#include "PluginEditor.h"

SequenceSequencerComponent::SequenceSequencerComponent(FxseqAudioProcessorEditor *ape,std::string ComboTemplate,std::string SliderTemplate,std::string ImageButtonTemplate)
{
    setSize (260, 150);
    comboTemplate=ComboTemplate;
    sliderTemplate=SliderTemplate;
    imageButtonTemplate=ImageButtonTemplate;

    APE=ape; 

    initSlider2(sequenceLengthSlider,2,16,1);
    sequenceLengthSlider.setValue(4);
    sequenceLengthSlider.onValueChange = [this] {changeSequenceLength();};
    
    for(int i=0;i<16;i++) {sequenceSelect.addItem(TRANS(std::__cxx11::to_string(i+1)),i+1);}
    for(int i=0;i<comboColors.size();i++) {  sequenceSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
    sequenceSelect.setSelectedItemIndex(0);
    sequenceSelect.onChange = [this] {changeSelectedSequence();};
    addAndMakeVisible (sequenceSelect);
    sequenceSelectLabel.setText("PATTERN",juce::dontSendNotification);
    //  addAndMakeVisible (sequenceSelectLabel);

    for (int j=0;j<16;j++) {
        for(int i=0;i<16;i++) {sequenceStep[j].addItem(TRANS(std::__cxx11::to_string(i+1)),i+1);}
        for(int i=0;i<comboColors.size();i++) {  sequenceStep[j].setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));   }
        sequenceStep[j].setSelectedItemIndex(0);
        addAndMakeVisible (sequenceStep[j]);
    }
    sequenceStep[0].onChange = [this] {sequenceChanged(0);};sequenceStep[4].onChange = [this] {sequenceChanged(4);};sequenceStep[8].onChange = [this] {sequenceChanged(8);};sequenceStep[12].onChange = [this] {sequenceChanged(12);};
    sequenceStep[1].onChange = [this] {sequenceChanged(1);};sequenceStep[5].onChange = [this] {sequenceChanged(5);};sequenceStep[9].onChange = [this] {sequenceChanged(9);};sequenceStep[13].onChange = [this] {sequenceChanged(13);};
    sequenceStep[2].onChange = [this] {sequenceChanged(2);};sequenceStep[6].onChange = [this] {sequenceChanged(6);};sequenceStep[10].onChange = [this] {sequenceChanged(10);};sequenceStep[14].onChange = [this] {sequenceChanged(14);};
    sequenceStep[3].onChange = [this] {sequenceChanged(3);};sequenceStep[7].onChange = [this] {sequenceChanged(7);};sequenceStep[11].onChange = [this] {sequenceChanged(11);};sequenceStep[15].onChange = [this] {sequenceChanged(15);};
}   

void SequenceSequencerComponent::resized()
{
    sequenceSelectLabel.setBounds         (5,0,60,30);
    sequenceSelect.setBounds              (5,15,60,30);
    sequenceLengthSliderLabel.setBounds   (60,0,100,30);
    sequenceLengthSlider.setBounds        (60,15,75,30);

    for (int j=0;j<16;j++) {
        sequenceStep[j].setBounds         (150+j*60,20,55,20);
    }
}

void SequenceSequencerComponent::skinChange()
{   
    for(int i=0;i<comboColors.size();i++) 
    {  
        sequenceSelect.setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));
        for (int j=0;j<16;j++) {sequenceStep[j].setColour(comboColors[i][0],juce::Colour(comboColors[i][1]));  }    
    }
    for(int i=0;i<sliderColors.size();i++) {
        sequenceLengthSlider.setColour(sliderColors[i][0],juce::Colour(sliderColors[i][1]));
    }
    sequenceLengthSliderLabel.setColour(sliderColors[2][0],juce::Colour(sliderColors[2][1]));
    sequenceSelectLabel.setColour(sliderColors[2][0],juce::Colour(sliderColors[2][1]));
}
////////////////////////////////////////////// TEMPLATES //////////////////////////////////////////////
void SequenceSequencerComponent::initSlider2(juce::Slider& slider,float min,float max,float def)
{   slider.setSliderStyle (juce::Slider::LinearHorizontal );
    slider.setRange (min, max, def);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 80, 20);       
    addAndMakeVisible(slider);
}
/////////////////////////////////////////////// CALLBACKS///////////////////////////////////////////////
void SequenceSequencerComponent::changeSelectedSequence()
{
    int SelectedSequence=sequenceSelect.getSelectedItemIndex();
    APE->changeSelectedSequence(SelectedSequence);
    sequence=APE->getSequence(SelectedSequence);
    for (int j=0;j<16;j++) {
        sequenceStep[j].setSelectedItemIndex(sequence[j]);
    }
    selectedSequence=SelectedSequence;
}

void SequenceSequencerComponent::changeSequenceLength()
{
    int sequenceLength=sequenceLengthSlider.getValue();
    APE->updateSequenceLength(sequenceLength);
    for (int i=0;i<16;i++) 
    {
        int mask=0xffffffff,mask2;
        if (i>sequenceLength-1) {
            mask=0xFF3F3F3F;
        } 
        for(int j=0;j<comboColors.size();j++) {
            if (j==1) {mask2=0xFF000000;} else {mask2=mask;}
            sequenceStep[i].setColour(comboColors[j][0],juce::Colour(comboColors[j][1] & mask2));
        }
        
    }
}

void SequenceSequencerComponent::sequenceChanged(int step)
{
    sequence[step]=sequenceStep[step].getSelectedItemIndex();
    APE->updateSequence(selectedSequence,sequence);
}


void SequenceSequencerComponent::updatePosition(int position)
{
    for(int i=0;i<getSequenceLength();i++)
    {
        int coeff;
        if (i==position) { coeff=0xFFFFFFFF; }
        else { coeff=0xFF7F7F7F; }
        sequenceStep[i].setColour(comboColors[1][0],juce::Colour(comboColors[1][1]&coeff));
        sequenceStep[i].setColour(comboColors[2][0],juce::Colour(comboColors[2][1]&coeff));        
        sequenceStep[i].setColour(comboColors[3][0],juce::Colour(comboColors[3][1]&coeff));
    }
}

int SequenceSequencerComponent::getSequenceLength()
{
    return sequenceLengthSlider.getValue();
}

#pragma once

#pragma once
#include "../../JuceLibraryCode/JuceHeader.h"

class Filter : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="Filter";
        std::vector<EffectParameter> params;

        void initEffectParams()
        {
            EffectParameter frequency =     { .id = "Frequency",   .name = "Frequency",     .faustName = "Frequency",   .type = "float",   .min=20.0f,   .max=20000.0f, .step=1.0f,    .value=20000.0f};
            EffectParameter resonance =     { .id = "Resonance",   .name = "Resonance",     .faustName = "Resonance",   .type = "float",   .min=0.0f,    .max=1.0f,     .step=1.0f,     .value=0.5f};
            EffectParameter drive     =     { .id = "Drive",       .name = "Drive",         .faustName = "Drive",       .type = "float",   .min=1.0f,    .max=10.0f,    .step=0.1f,     .value=0.0f};

            params.push_back(frequency);
            params.push_back(resonance);
            params.push_back(drive);
        };    

	    Filter() 
        { 
            initEffectParams(); 
            filter.reset();
            //filter.setMode(juce::dsp::LadderFilter<float>::Mode::LowPass);
            /*filter.setCutoffFrequencyHz(20000.0f);
            filter.setResonance(0.0f);*/
        };
	    ~Filter() {};

	    void prepareToPlay(double sampleRate, int samplesPerBlock)
        {
            dryBuffer.setSize(2, samplesPerBlock);
            filter.reset();
            //filter.setSampleRate(sampleRate);
            //filter.prepare({ sampleRate, (double)maximumBlockSize, (double)getTotalNumInputChannels() });
        };

        void releaseResources(){};

	    void processBlock(juce::AudioBuffer<float>& buffer, const int numSamples,float gain,float mix,float sequencerGate)
        {
            /*float frequency = fxPrograms[2][patterns[2][selected_pattern[2]][int(sequencerPositions[2]/resolution)]][0];// FX4
            float resonance = fxPrograms[2][patterns[2][selected_pattern[2]][int(sequencerPositions[2]/resolution)]][1];// FX4 
            float drive=  fxPrograms[2][patterns[2][selected_pattern[2]][int(sequencerPositions[2]/resolution)]][2];*/
            float wet = gain * mix;
            float dry = (1.0f - mix);
            filterFreq_smoothed.setTargetValue(params[0].value);            

            dryBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());// copy dry input   
            dryBuffer.copyFrom(1, 0, buffer, 1, 0, buffer.getNumSamples());// copy dry input   
            
            juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(0), dry, buffer.getNumSamples());
	        juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(1), dry, buffer.getNumSamples());

            juce::dsp::AudioBlock<float> audioBlock(buffer); 
            juce::dsp::ProcessContextReplacing<float> context(audioBlock);
            filter.process(context);

	        juce::FloatVectorOperations::multiply(buffer.getWritePointer(0), wet, buffer.getNumSamples());
	        juce::FloatVectorOperations::multiply(buffer.getWritePointer(1), wet, buffer.getNumSamples());

	        juce::FloatVectorOperations::add(buffer.getWritePointer(0), dryBuffer.getReadPointer(0), buffer.getNumSamples());
	        juce::FloatVectorOperations::add(buffer.getWritePointer(1), dryBuffer.getReadPointer(1), buffer.getNumSamples());    
	    };

        void changeParameter(const int paramIndex, float newValue)
	    {       
            if ( paramIndex < params.size() )
            {
                if (params[paramIndex].value != newValue)
                {   
                    std::string paramName=params[paramIndex].name;                              
                    if (paramName =="Frequency" ) {
                        filterFreq_smoothed.setTargetValue(newValue);
                        filter.setCutoffFrequencyHz(filterFreq_smoothed.getNextValue());
                    }
                    if (paramName =="Resonance" ) {
                        filter.setResonance(newValue);
                    }

                    if (paramName =="Drive" ) {
                        filter.setDrive(newValue);
                    }
                    params[paramIndex].value = newValue;
                }      
            }
		    
	    };

    private:
        void parameterChanged(const juce::String& paramID, float newValue) {}; 

        juce::dsp::LadderFilter<float> filter;
        juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative>    filterFreq_smoothed;  
        juce::AudioBuffer<float> dryBuffer;
};

#pragma once

#pragma once
#include <JuceHeader.h>
#include "FaustEffects.h"

class CombFilter : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="FaustCombFilter";
        int inputCount=1;
        int outputCount=1;
        std::vector<EffectParameter> params;

        void initEffectParams()
        {
            EffectParameter time =     { .id = "Frequency",   .name = "Frequency",     .faustName = "delayLength",       .type = "float",   .min=0.0f,    .max=1000.0f,.step=0.1f,     .value=50.0f};
            EffectParameter feedback = { .id = "Feedback",    .name = "Feedback",      .faustName = "feedback",          .type = "float",   .min=0.0f,    .max=0.99f,  .step=0.1f,     .value=0.3f};

            params.push_back(time);
            params.push_back(feedback);
        };    

	    CombFilter() { initEffectParams(); };
	    ~CombFilter() {};

	    void prepareToPlay(double sampleRate, int samplesPerBlock)
	    {
            DSP = new cfdsp();
            DSP->init(sampleRate);
            UI = new MapUI();
            DSP->buildUserInterface(UI);
            Inputs = new float*[inputCount];
            Outputs = new float*[outputCount];
            for (int channel = 0; channel < inputCount; ++channel) {
                Inputs[channel] = new float[samplesPerBlock];
            }
            for (int channel = 0; channel < outputCount; ++channel) {
                Outputs[channel] = new float[samplesPerBlock];
            }
            wetBuffer.setSize(2, samplesPerBlock);
	    };

        void releaseResources()
        {
            delete DSP;
            delete UI;
            for (int channel = 0; channel < inputCount; ++channel) {
                delete[] Inputs[channel];
            }
            for (int channel = 0; channel < outputCount; ++channel) {
                delete[] Outputs[channel];
            }

            delete [] Inputs;
            delete [] Outputs;
        };

	    void processBlock(juce::AudioBuffer<float>& buffer, const int numSamples,float gain,float mix,float sequencerGate)
        {
            float lastGainValue=smoothMix.getNextValue();
            smoothMix.setTargetValue(sequencerGate*mix);
            float volume=smoothMix.getNextValue();

            for (int channel = 0; channel < inputCount; ++channel) {
                for (int i = 0; i < numSamples; i++) { 
                        Inputs[channel][i] = *buffer.getWritePointer(channel,i); 
                    }
            }
            DSP->compute(numSamples,Inputs,Outputs);
            for (int channel = 0; channel < 2; ++channel) {
                for (int i = 0; i < numSamples; i++){
                    *wetBuffer.getWritePointer(channel,i) =gain*Outputs[0][i]*sequencerGate*mix + (1-mix*sequencerGate) * buffer.getReadPointer(channel)[i];  
                }
                wetBuffer.applyGainRamp(channel,0,numSamples,lastGainValue,volume);
                buffer.applyGainRamp(channel,0,numSamples,1.0f-lastGainValue,1.0f-volume);
            
                juce::FloatVectorOperations::add(buffer.getWritePointer(channel), wetBuffer.getReadPointer(channel), numSamples);
            }
		    
	    };

        void changeParameter(const int paramIndex, float newValue)
	    {       
            if ( paramIndex < params.size() )
            {
                if (params[paramIndex].value != newValue)
                {

                    if (params[paramIndex].type == "int") {UI->setParamValue(params[paramIndex].faustName,int(newValue));} 
                    else { UI->setParamValue(params[paramIndex].faustName,newValue);}
                
                    params[paramIndex].value=newValue;  
                }      
            }
		    
	    };

    private:
        void parameterChanged(const juce::String& paramID, float newValue) {};   
        MapUI* UI;
        dsp* DSP;
        float **Inputs;
        float **Outputs;
        juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> smoothMix;
        juce::AudioBuffer<float> wetBuffer;

};

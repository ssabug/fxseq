#pragma once

#pragma once
#include <JuceHeader.h>
#include "FaustEffects.h"

class Echo : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="FaustEcho";
        int inputCount=2;
        int outputCount=2;
        std::vector<EffectParameter> params;

        void initEffectParams()
        {
            EffectParameter time =     { .id = "Time",     .name = "Time",     .faustName = "delay",           .type = "float",   .min=0.0f,    .max=1.0f,   .step=0.1f,     .value=0.25f};
            EffectParameter feedback = { .id = "Feedback", .name = "Feedback", .faustName = "feedback",        .type = "float",   .min=0.0f,    .max=1.0f,   .step=0.1f,     .value=0.5f};

            params.push_back(time);
            params.push_back(feedback);
        };    

	    Echo() { initEffectParams(); };
	    ~Echo() {};

	    void prepareToPlay(double sampleRate, int samplesPerBlock)
	    {
            DSP = new mydsp();
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
            for (int channel = 0; channel < 2; ++channel) {
                for (int i = 0; i < numSamples; i++) { 
                        Inputs[channel][i] = *buffer.getWritePointer(channel,i); 
                    }
            }
            DSP->compute(numSamples,Inputs,Outputs);
            for (int channel = 0; channel < 2; ++channel) {
                for (int i = 0; i < numSamples; i++){
                    *buffer.getWritePointer(channel,i) =gain*Outputs[channel][i]*sequencerGate*mix + (1-mix*sequencerGate) * Inputs[channel][i];  
                }
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

};

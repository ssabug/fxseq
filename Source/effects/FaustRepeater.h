#pragma once

#pragma once
#include "../../JuceLibraryCode/JuceHeader.h"
#include "FaustEffects.h"

class Repeater : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="FaustRepeater";
        int inputCount=2;
        int outputCount=2;
        std::vector<EffectParameter> params;

        void initEffectParams()
        {
            EffectParameter msize = { .id = "mSize", .name = "mSize",   .faustName = "MasterTaille",    .type = "float", .min=200.0f,    .max=2000.0f,  .step=0.1f,     .value=1000.0f};
            EffectParameter count = { .id = "count", .name = "count",   .faustName = "nbRepet",         .type = "int",   .min=1.0f,      .max=16.0f,    .step=1.0f,     .value=4.0f};
            EffectParameter size =  { .id = "size",  .name = "size",    .faustName = "taille",          .type = "float", .min=2.0f,      .max=200.0f,   .step=0.1f,     .value=100.0f};

            params.push_back(msize);
            params.push_back(count);
            params.push_back(size);
        };    

	    Repeater() { initEffectParams(); };
	    ~Repeater() {};

	    void prepareToPlay(double sampleRate, int samplesPerBlock)
	    {
            DSP = new rpdsp();
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

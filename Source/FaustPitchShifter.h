#pragma once

#pragma once
#include <JuceHeader.h>
#include "FaustEffects.h"

class PitchShifter : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="FaustPitchShifter";
        int inputCount=1;
        int outputCount=1;
        std::vector<EffectParameter> params;
        
        PitchShifter() { initEffectParams(); };
	    ~PitchShifter() {};

        void initEffectParams()
        {
            EffectParameter time =     { .id = "Frequency",.name = "Frequency",  .faustName = "pitch",       .type = "float",   .min=-12.0f,    .max=12.0f,     .step=0.1f,     .value=0.0f};
            EffectParameter feedback = { .id = "Window",   .name = "Window",     .faustName = "window",      .type = "int",     .min=50.0f,     .max=10000.99f, .step=1.0f,     .value=50.0f};
            EffectParameter xfade    = { .id = "Xfade",    .name = "Xfade",      .faustName = "xfade",       .type = "int",     .min=1.0f,      .max=10000.0f,  .step=1.0f,     .value=1.0f};

            params.push_back(time);
            params.push_back(feedback);
            params.push_back(xfade);
        };    

	    void prepareToPlay(double sampleRate, int samplesPerBlock)
	    {
            DSP = new psdsp();
            DSP->init(sampleRate);
            UI = new MapUI();
            DSP->buildUserInterface(UI);
            Inputs =  new float*[inputCount];
            Outputs = new float*[outputCount];
            for (int channel = 0; channel < inputCount; ++channel) {
                Inputs[channel] = new float[samplesPerBlock];
            }
            for (int channel = 0; channel < outputCount; ++channel) {
                Outputs[channel] = new float[samplesPerBlock];
            }

            wetBuffer.setSize(2, samplesPerBlock); 
            /*smoothMix.reset  (sampleRate,0.5);
            smoothFreq.reset (sampleRate,0.5);*/
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
            float pitch=params[0].value;
            lastGainValue=smoothMix.getNextValue();
            smoothMix.setTargetValue(std::floor(sequencerGate+0.95f)*mix);
            //mix=smoothMix.getNextValue();
            

            if (pitch >= 0)
            {
                pitch-=(1.0-sequencerGate)*12;
            } else {
                pitch+=(1.0-sequencerGate)*12;
            }
            smoothFreq.setTargetValue(pitch);
            changeParameter(0,smoothFreq.getNextValue());

            for (int channel = 0; channel < inputCount; ++channel) {
                for (int i = 0; i < numSamples; i++) { 
                        Inputs[0][i] = *buffer.getWritePointer(channel,i); 
                    }
            }

            DSP->compute(numSamples,Inputs,Outputs);        
            float volume=smoothMix.getNextValue();
            for (int channel = 0; channel < 2; ++channel) {
                for (int i = 0; i < numSamples; i++){
                    *wetBuffer.getWritePointer(channel,i) = gain*Outputs[0][i]*mix  /* +   (1-mix) * Inputs[0][i]*/;  
                }
                wetBuffer.applyGainRamp(channel,0,numSamples,lastGainValue,volume);
                buffer.applyGainRamp(channel,0,numSamples,1.0f-lastGainValue,1.0f-volume);
            
                juce::FloatVectorOperations::add(buffer.getWritePointer(channel), wetBuffer.getReadPointer(channel), numSamples);
                //buffer.applyGainRamp(channel,0,sampleSize,lastFxDepths[0],smoothMix.getNextValue()));  // chopper             
            }
		    
	    };

        void changeParameter(const int paramIndex, float newValue)
	    {       
            if ( paramIndex < params.size() )
            {
                if (params[paramIndex].value != newValue)
                {
                    newValue=std::clamp(newValue,params[paramIndex].min,params[paramIndex].max);

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
        juce::SmoothedValue<float,juce::ValueSmoothingTypes::Multiplicative> smoothMix,smoothFreq;
        float lastGainValue;
        juce::AudioBuffer<float> wetBuffer;

};

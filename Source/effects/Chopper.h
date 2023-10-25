#pragma once

#pragma once
#include "../../JuceLibraryCode/JuceHeader.h"

class Chopper : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="Chopper";
        std::vector<EffectParameter> params;

        void initEffectParams()
        {
           
        };    

	    Chopper() { initEffectParams(); };
	    ~Chopper() {};

	    void prepareToPlay(double sampleRate, int samplesPerBlock){ dryBuffer.setSize(2, samplesPerBlock); };

        void releaseResources(){};

	    void processBlock(juce::AudioBuffer<float>& buffer, const int numSamples,float gain,float mix,float sequencerGate,float lastFxDepth)
        {
            
            int sampleSize=buffer.getNumSamples();
            float wet = gain * mix;
            float dry = ( 1.0f - mix);


            for (int channel = 0; channel < 2; ++channel) {
                dryBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());// copy dry input 
                buffer.applyGainRamp(channel,0,numSamples,lastFxDepth,sequencerGate);  // chopper      
                
                juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(channel), dry, numSamples);
                juce::FloatVectorOperations::multiply(buffer.getWritePointer(channel), wet, numSamples);   
                juce::FloatVectorOperations::add(buffer.getWritePointer(channel), dryBuffer.getReadPointer(channel), numSamples);
            }


            /*juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(0), dry, numSamples);
	        juce::FloatVectorOperations::multiply(dryBuffer.getWritePointer(1), dry, numSamples);

	        juce::FloatVectorOperations::multiply(buffer.getWritePointer(0), wet, numSamples);
	        juce::FloatVectorOperations::multiply(buffer.getWritePointer(1), wet, numSamples);

	        juce::FloatVectorOperations::add(buffer.getWritePointer(0), dryBuffer.getReadPointer(0), numSamples);
	        juce::FloatVectorOperations::add(buffer.getWritePointer(1), dryBuffer.getReadPointer(1), numSamples);  */ 
	    };

        void changeParameter(const int paramIndex, float newValue)
	    {       
            if ( paramIndex < params.size() )
            {
                if (params[paramIndex].value != newValue)
                {                                  
                    params[paramIndex].value=newValue;  
                }      
            }
		    
	    };

    private:
        void parameterChanged(const juce::String& paramID, float newValue) {};   
        juce::AudioBuffer<float> dryBuffer;
};

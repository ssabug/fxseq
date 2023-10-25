#pragma once

#pragma once
#include "../../JuceLibraryCode/JuceHeader.h"

class TimeStretcher : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="TimeStretcher";
        std::vector<EffectParameter> params;

        void initEffectParams()
        {
            EffectParameter rate =    { .id = "rate",  .name = "rate",    .faustName = "rate",  .type = "float",   .min=0.01f,    .max=32.0f,     .step=0.01f,     .value=1.0f};

            params.push_back(rate);
        };    

	    DirtyRepeater() 
        { 
            initEffectParams();
            
        };
	    ~DirtyRepeater() {};

	    void prepareToPlay(double sampleRate, int samplesPerBlock)
        {
            // Update the sample rate and block size
            rate = sampleRate;
            size = samplesPerBlock;

            // Reset the phase and buffer
            phase = 0;
            buffer.setSize(2, size);
            buffer.clear();
        };

        void releaseResources()
        {

        };

	    void processBlock(juce::AudioBuffer<float>& buffer, const int numSamples,float gain,float mix,float sequencerGate,float lastGate,double bpm, double ppq)
        {
            const float* inData = buffer.getReadPointer(0);
            float* outData = buffer.getWritePointer(0);

            // Calculate the stretch factor based on the desired rate
            double stretchFactor = rate / getSampleRate();

            for (int sample = 0; sample < size; ++sample)
            {
                // Calculate the current position in the input buffer
                double position = phase * stretchFactor;
                int index = (int)position;
                float alpha = (float)(position - index);

                // Perform linear interpolation between adjacent samples
                float inputSample1 = inData[index];
                float inputSample2 = inData[index + 1];
                float outputSample = inputSample1 + alpha * (inputSample2 - inputSample1);

                // Write the output sample to the buffer
                outData[sample] = outputSample;

                // Advance the phase
                phase += 1.0 / stretchFactor;
        }
	    };

        void changeParameter(const int paramIndex, float newValue)
	    {       
            if ( paramIndex < params.size() )
            {
                if (params[paramIndex].value != newValue)
                {  
                    newValue=std::clamp(newValue,params[paramIndex].min,params[paramIndex].max);
                     
                    if (params[paramIndex].type == "int")
                    {
                        params[paramIndex].value = int(newValue);
                    } else {
                        params[paramIndex].value = newValue;
                    }
                }      
            }
		    
	    };

    private:
        void parameterChanged(const juce::String& paramID, float newValue) {}; 

        double rate;  // Desired time stretch rate
        int size;     // Block size
        double phase; // Current phase position
        juce::AudioBuffer<float> buffer; // Input buffer
};

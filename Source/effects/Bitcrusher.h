#pragma once

#pragma once
#include <JuceHeader.h>

class Bitcrusher : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="Crusher";
        std::vector<EffectParameter> params;

        void initEffectParams()
        {
            EffectParameter reduction =    { .id = "Reduction",  .name = "Reduction",    .faustName = "Reduction",  .type = "int",   .min=0.0f,    .max=30.0f,     .step=1.0f,     .value=0.0f};
            EffectParameter bitDepth =     { .id = "Bitdepth",   .name = "Bitdepth",     .faustName = "Bitdepth",   .type = "int",   .min=1.0f,    .max=32.0f,     .step=1.0f,     .value=32.0f};

            params.push_back(reduction);
            params.push_back(bitDepth);
        };    

	    Bitcrusher() { initEffectParams(); };
	    ~Bitcrusher() {};

	    void prepareToPlay(double sampleRate, int samplesPerBlock){};

        void releaseResources(){};

	    void processBlock(juce::AudioBuffer<float>& buffer, const int numSamples,float gain,float mix,float sequencerGate)
        {
            int reduction = params[0].value ;
            int bitDepth = params[1].value ;
            
            for (int channel = 0; channel < 2; ++channel) {
                float* channelData = buffer.getWritePointer(channel);
                for (int i = 0; i < buffer.getNumSamples(); i++){

                    float totalQLevels =powf(2, bitDepth);
			        float val = channelData[i];
                    
			        channelData[i] =val*(1-sequencerGate*mix) + gain * (val - std::fmod(val, 1/totalQLevels )) * sequencerGate*mix;// Bit Quantizing      
         
                     if (reduction > 1) {if (i%reduction != 0) channelData[i] = sequencerGate*mix*channelData[i - i%reduction]+(1-sequencerGate*mix)*channelData[i];} // sample rate reduction

                }
            }
	    };

        void changeParameter(const int paramIndex, float newValue)
	    {       
            if ( paramIndex < params.size() )
            {
                if (params[paramIndex].value != newValue)
                {   
                    
                    params[paramIndex].value = newValue;
                }      
            }
		    
	    };

    private:
        void parameterChanged(const juce::String& paramID, float newValue) {}; 

};

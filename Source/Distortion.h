#pragma once

#pragma once
#include <JuceHeader.h>

class Distortion : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="Distortion";
        std::vector<EffectParameter> params;

        void initEffectParams()
        {
            EffectParameter type =     { .id = "Type",   .name = "Type",     .faustName = "Type",   .type = "int",   .min=0.0f,    .max=1.0f,.step=1.0f,     .value=0.0f};

            params.push_back(type);

        };    

	    Distortion() { initEffectParams(); };
	    ~Distortion() {};

	    void prepareToPlay(double sampleRate, int samplesPerBlock){};

        void releaseResources(){};

	    void processBlock(juce::AudioBuffer<float>& buffer, const int numSamples,float gain,float mix,float sequencerGate)
        {
            if (params[0].value == 0) 
            {
                classic_distortion(buffer,numSamples,gain,mix,sequencerGate);
            }
            
		    if (params[0].value == 1) 
            {
                hardclip_distortion(buffer,numSamples,gain,mix,sequencerGate);
            }
	    };

        void classic_distortion(juce::AudioBuffer<float>& buffer, const int numSamples,float gain,float mix,float sequencerGate)
        {
            const float drive=0.9f;
            mix*=sequencerGate;
            const float range=10.0f;
            gain*=1.0f;


            for (int channel = 0; channel < 2; ++channel)
            {
                auto* channelData = buffer.getWritePointer (channel);
                for (int sample = 0; sample < buffer.getNumSamples(); sample++){
                    float cleanSig = *channelData;
                    *channelData *= drive*range;
                    *channelData = gain*((((2.0f/juce::MathConstants<float>::pi) * atan(*channelData))*mix*(range/40.0f))+(cleanSig * (1.0-mix)));
                    channelData++;
                }
            }
        }

        void hardclip_distortion(juce::AudioBuffer<float>& buffer, const int numSamples,float gain,float mix,float sequencerGate)
        {
            mix*=sequencerGate;
            gain*=1.0f;
            float saturationAmount = 100.0f * 0.7f;
	        for (int channel = 0; channel < 2; ++channel)
            {
                auto *channelData = buffer.getWritePointer(channel);

                for (int sample = 0; sample < buffer.getNumSamples(); sample++)
                {
                    float xn = channelData[sample];
                    // hard clipping 
                    channelData[sample] = gain*mix*0.25*std::min(1.f, std::max(-1.f, xn * (saturationAmount * 0.06f + 1.f)))+ (1-mix)*channelData[sample]; // multiply audio on both channels by gain
                }
            }
        }

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

};

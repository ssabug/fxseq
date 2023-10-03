#pragma once

#pragma once
#include <JuceHeader.h>

class DirtyRepeater : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        std::string name="DirtyRepeater";
        std::vector<EffectParameter> params;

        void initEffectParams()
        {
            EffectParameter size =    { .id = "size",  .name = "size",    .faustName = "size",  .type = "int",   .min=1.0f,    .max=4.0f,     .step=1.0f,     .value=1.0f};

            params.push_back(size);
        };    

	    DirtyRepeater() 
        { 
            initEffectParams();
            /*buffer_smoothed[0].setTargetValue(0.0); 
            buffer_smoothed[1].setTargetValue(0.0); */
        };
	    ~DirtyRepeater() {};

	    void prepareToPlay(double sampleRate, int samplesPerBlock)
        {
            repeaterBuffer.setSize(2, samplesPerBlock); 
            crossfadeBuffer.setSize(2, samplesPerBlock); 
            repeater_buffer.clear();
            buffer_pos=0;        
        };

        void releaseResources()
        {
            repeater_buffer.clear();
            buffer_pos=0;
        };

	    void processBlock(juce::AudioBuffer<float>& buffer, const int numSamples,float gain,float mix,float sequencerGate,float lastGate,double bpm, double ppq)
        {
            int repeatSize=int( numSamples * (60.0 / (float) (bpm) * 8.0/*params[0].value*/ ));/*params[0].value));*/
            mix*=sequencerGate;
            double smooth;
            double lastValue[2];
            int crossFadeStart=1;
            
            if (repeater_buffer.size() != 2) {
                repeater_buffer.resize(2);
                for (int i = 0; i < repeater_buffer.size(); i++) {
                    repeater_buffer[i].clear();
                }
                buffer_pos=0;
            }

            if (std::modf(ppq/4,&unused) < 0.02f) 
            {
                for (int i = 0; i < repeater_buffer.size(); i++) {
                    repeater_buffer[i].clear();
                }
                buffer_pos=0;
            }
            
            double mmix=sequencerGate;//fxDepths_smoothed[5].getNextValue();

            lastValue[0]=buffer_smoothed[0].getNextValue();
            lastValue[1]=buffer_smoothed[1].getNextValue();

            if (buffer_pos==0) {                    
                    buffer_smoothed[0].setTargetValue(1.0);
                    buffer_smoothed[1].setTargetValue(1.0);
                }
            if (buffer_pos>repeatSize-crossFadeStart*numSamples) {
                buffer_smoothed[0].setTargetValue(0.0);
                buffer_smoothed[1].setTargetValue(0.0);
            }
            double crossVolume=buffer_smoothed[0].getNextValue();

            for (int channel = 0; channel < 2; ++channel) 
            {
                auto* channelData = buffer.getWritePointer(channel);
                auto* repeatData=  repeaterBuffer.getWritePointer(channel);
                auto* startData=  crossfadeBuffer.getWritePointer(channel);
                
                for (int sample = 0; sample < numSamples; sample++)
                {
                    repeater_buffer[channel].insert(repeater_buffer[channel].end(),channelData[sample]);                                    
                    startData[sample]=repeater_buffer[channel][sample];
                    repeatData[sample]=repeater_buffer[channel][buffer_pos+sample];

                }

                //if (buffer_pos==0) {
                    repeaterBuffer.applyGainRamp(channel,0,numSamples,lastValue[channel],crossVolume); // crossfade current buffer and start buffer
                    //crossfadeBuffer.applyGainRamp(channel,0,numSamples,0,crossVolume);
                    //juce::FloatVectorOperations::add(repeaterBuffer.getWritePointer(channel), crossfadeBuffer.getReadPointer(channel), numSamples);
                //}

                repeaterBuffer.applyGainRamp(channel,0,numSamples,lastGate,mmix);
                buffer.applyGainRamp(channel,0,numSamples,1.0f-lastGate,1.0f-mmix);
                juce::FloatVectorOperations::add(buffer.getWritePointer(channel), repeaterBuffer.getReadPointer(channel), numSamples);
            }
          
            if (buffer_pos < repeatSize ) {
                buffer_pos+=numSamples;
            } else {
                buffer_pos=0;
            }
	    };

        void changeParameter(const int paramIndex, float newValue)
	    {       
            if ( paramIndex < params.size() )
            {
                if (params[paramIndex].value != newValue)
                {   
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
        
        std::vector<std::vector<float>> repeater_buffer;
        juce::AudioBuffer<float> repeaterBuffer,crossfadeBuffer;
        int buffer_pos=0;
        float unused;
        juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear> buffer_smoothed[2];
        juce::SmoothedValue<float,juce::ValueSmoothingTypes::Linear> interpolated[2]; 
};

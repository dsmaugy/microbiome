/*
  ==============================================================================

    MicrobiomeEngine.h
    Created: 18 Mar 2024 11:27:56am
    Author:  darwin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"
#include "Colony.h"
#include "GlobalDelay.h"

struct EngineParams
{
    juce::dsp::ProcessSpec procSpec;
};

class MicrobiomeEngine 
{
    public:
        MicrobiomeEngine(juce::AudioProcessorValueTreeState&);

        void prepare(const EngineParams& params);
        void processAudio(juce::AudioBuffer<float>& buffer);

        void disableColony(int n);
        void enableColony(int n );

        // removes from end of colony array to beginning
        void removeColony();
        // adds from beginning of colony array to end
        void addColony();

        void setColonyDelayTime(int n, float sec);
        void setColonyResampleRatio(int n, float ratio);
        void setColonyResampleStart(int n, float start);
        void setColonyBufferStart(int n, float startSec);
        void setColonyBufferLength(int n, float lengthSec);
        void setBPM(double bpm);

    private:
        juce::AudioProcessorValueTreeState& parameters;
        EngineParams params;

        std::array<std::unique_ptr<Colony>, MAX_COLONY> colony;

        juce::dsp::Reverb reverb;
        juce::dsp::Reverb::Parameters reverbParameters;
        //std::unique_ptr<juce::AudioBuffer<float>> delayBuffer;
        std::unique_ptr<GlobalDelay> delayBuffer;
        
        int delayWriteIdx = 0;
        // TODO: this is just for debuggin
        int delayReadIdx = 0;

        double currentBpm = 0;
};
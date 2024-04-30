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

        void setBPM(double bpm);

    private:
        juce::AudioProcessorValueTreeState& parameters;
        EngineParams params;

        std::array<std::unique_ptr<Colony>, MAX_COLONY> colony;

        juce::dsp::Reverb reverb;
        juce::dsp::Reverb::Parameters reverbParameters;
        std::unique_ptr<juce::AudioBuffer<float>> delayBuffer;
        
        int delayWriteIdx = 0;
        // TODO: this is just for debuggin
        int delayReadIdx = 0;

        double currentBpm = 0;
};
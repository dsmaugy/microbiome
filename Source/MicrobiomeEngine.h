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
        MicrobiomeEngine();

        void prepare(const EngineParams& params);
        void processAudio(juce::AudioBuffer<float>& buffer);

        void disableColony(int n);
        void enableColony(int n );

        // removes from end of colony array to beginning
        void removeColony();
        // adds from beginning of colony array to end
        void addColony();

    private:
        Colony colony[MAX_COLONY];
        int activeColonies;
        EngineParams params;
};
/*
  ==============================================================================

    MicrobiomeEngine.cpp
    Created: 18 Mar 2024 11:27:56am
    Author:  darwin

  ==============================================================================
*/

#include "MicrobiomeEngine.h"

#define DEFAULT_COLONIES 3

MicrobiomeEngine::MicrobiomeEngine() 
{
    activeColonies = DEFAULT_COLONIES;
}


void MicrobiomeEngine::prepare(const EngineParams& params)
{
    this->params = params;

    for (int i = 0; i < MAX_COLONY; i++) {
        ColonyParams colonyParams;
        colonyParams.procSpec = params.procSpec;

        if (i < DEFAULT_COLONIES) {
            colonyParams.delayInSamples = 10 * params.procSpec.sampleRate * (1.0/(i+1));
            colony[i].setActive(true);
        }
        colony[i].prepare(colonyParams);
    }   
}

void MicrobiomeEngine::processAudio(juce::AudioBuffer<float>& buffer)
{
    for (int i = 0; i < activeColonies; i++) {

    }

    for (int channel = 0; channel < buffer.getNumChannels() && channel < MAX_CHANNELS; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            for (int j = 0; j < activeColonies; j++) {
                
            }
        }
    }
}
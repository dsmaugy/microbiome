/*
  ==============================================================================

    MicrobiomeEngine.cpp
    Created: 18 Mar 2024 11:27:56am
    Author:  darwin

  ==============================================================================
*/

#include "MicrobiomeEngine.h"
#include "Constants.h"

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
    int appliedColonies = 0;
    for (int i = 0; i < MAX_COLONY && appliedColonies < activeColonies; i++) {
        appliedColonies++;
        colony[i].processAudio(buffer);
    }

    for (int channel = 0; channel < buffer.getNumChannels() && channel < MAX_CHANNELS; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);
    
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            appliedColonies = 0;
            for (int j = 0; j < MAX_COLONY && appliedColonies < activeColonies; j++) {
                if (colony[j].isActive()) {
                    appliedColonies++;
                    channelData[i] += colony[j].getGain() * colony[j].getSampleN(channel, i);
                }
            }
        }
    }
}
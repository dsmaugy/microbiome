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
            colony[i].toggleState(true);
        }
        colony[i].prepare(colonyParams);
    }   
}

void MicrobiomeEngine::processAudio(juce::AudioBuffer<float>& buffer)
{
    for (int i = 0; i < MAX_COLONY; i++) {
        if (colony[i].isActive()) {
            colony[i].processAudio(buffer);
        }
    }

    for (int channel = 0; channel < buffer.getNumChannels() && channel < MAX_CHANNELS; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);
    
        for (int i = 0; i < buffer.getNumSamples(); i++) {
            for (int j = 0; j < MAX_COLONY; j++) {
                // can't break out early from checking appliedColonies because some colonies may be ramping down
                if (colony[j].isActive()) {
                    // channelData[i] += colony[j].getSampleN(channel, i);
                }
            }
        }
    }
}

void MicrobiomeEngine::disableColony(int n)
{
    if (colony[n].isAlive()) {
        colony[n].toggleState(false);
        activeColonies--;

        DBG("Active Colonies: " << activeColonies);
    }
}

void MicrobiomeEngine::enableColony(int n)
{
    if (!colony[n].isAlive()) {
        colony[n].toggleState(true);
        activeColonies++;
    }
}

void MicrobiomeEngine::removeColony()
{
    for (int i = MAX_COLONY-1; i >= 0; i--) {
        if (colony[i].isAlive()) {
            disableColony(i);
            DBG("Active Colonies: " << activeColonies);
            break;
        }
    }
}

void MicrobiomeEngine::addColony()
{
    for (int i = 0; i < MAX_COLONY; i++) {
        if (!colony[i].isAlive()) {
            enableColony(i);
            DBG("Active Colonies: " << activeColonies);
            break;
        }
    }
}
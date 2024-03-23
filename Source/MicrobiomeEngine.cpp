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

    delayBuffer = std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, MAX_DELAY_SECONDS * params.procSpec.sampleRate);

    for (int i = 0; i < MAX_COLONY; i++) {
        ColonyParams colonyParams(params.procSpec, delayBuffer.get());

        if (i < DEFAULT_COLONIES) {
            // colonyParams.initialDelayInSamples = 10 * params.procSpec.sampleRate * (1.0/(i+1));
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

    int delayWriteStart = delayWriteIdx;
    int delayReadStart = delayReadIdx;
    for (int channel = 0; channel < buffer.getNumChannels() && channel < MAX_CHANNELS; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);
        delayWriteIdx = delayWriteStart;
        delayReadIdx = delayReadStart;

        for (int i = 0; i < buffer.getNumSamples(); i++) {
            float originalSample = channelData[i];
            for (int j = 0; j < MAX_COLONY; j++) {
                // can't break out early from checking appliedColonies because some colonies may be ramping down
                if (colony[j].isActive()) {
                     channelData[i] += colony[j].getSampleN(channel, i);
                    // channelData[i] = colony[j].getSampleN(channel, i);
                    // channelData[i] += delayBuffer->getSample(channel, delayReadIdx);
                }
            }
            delayReadIdx = (delayReadIdx+1) % 22050;// ((int) params.procSpec.sampleRate) * 5;
            // we update the delay after the current output sample has already been updated
            delayBuffer->setSample(channel, delayWriteIdx, originalSample);
            delayWriteIdx = (delayWriteIdx+1) % delayBuffer->getNumSamples();

            // DBG("Read Idx: " << delayReadIdx << "\tWrite Idx: " << delayWriteIdx);
            // DBG(delayBuffer->getSample(channel, delayReadIdx));
        }
    }
}

void MicrobiomeEngine::disableColony(int n)
{
    if (colony[n].isAlive()) {
        DBG("Disabling Colony: " << n);
        colony[n].toggleState(false);
        activeColonies--;
    }
}

void MicrobiomeEngine::enableColony(int n)
{
    if (!colony[n].isAlive()) {
        DBG("Enabling Colony: " << n);
        colony[n].toggleState(true);
        activeColonies++;
    }
}

void MicrobiomeEngine::removeColony()
{
    for (int i = MAX_COLONY-1; i >= 0; i--) {
        if (colony[i].isAlive()) {
            disableColony(i);
            break;
        }
    }
    DBG("Active Colonies: " << activeColonies);
}

void MicrobiomeEngine::addColony()
{
    for (int i = 0; i < MAX_COLONY; i++) {
        if (!colony[i].isAlive()) {
            enableColony(i);
            break;
        }
    }
    DBG("Active Colonies: " << activeColonies);
}

void MicrobiomeEngine::setColonyDelayTime(int n, float sec)
{
    // colony[n].setDelayTime(sec);
}

void MicrobiomeEngine::setColonyResampleRatio(int n, float ratio)
{
    colony[n].setResampleRatio(ratio);
}
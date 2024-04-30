/*
  ==============================================================================

    MicrobiomeEngine.cpp
    Created: 18 Mar 2024 11:27:56am
    Author:  darwin

  ==============================================================================
*/

#include "MicrobiomeEngine.h"
#include "Constants.h"

MicrobiomeEngine::MicrobiomeEngine(juce::AudioProcessorValueTreeState& p) : parameters(p), reverbParameters{0.7f, 0.25f, 0.33f, 0.4f, 1.0f, 0.0f}
{
    for (int i = 0; i < MAX_COLONY; i++) {
        colony[i] = std::make_unique<Colony>(i, parameters);
    }
    reverb.setParameters(reverbParameters);

}


void MicrobiomeEngine::prepare(const EngineParams& params)
{
    this->params = params;

    if (!delayBuffer || delayBuffer->getNumSamples() != MAX_DELAY_SECONDS * (int) params.procSpec.sampleRate) {
        DBG("Regenerating new Engine buffer");
        delayBuffer = std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, MAX_DELAY_SECONDS * (int) params.procSpec.sampleRate);
        // without clearing there are audible pops at initialization with using IIR filters
        delayBuffer->clear();
    }

    reverb.prepare(params.procSpec);

    for (int i = 0; i < MAX_COLONY; i++) {
        ColonyParams colonyParams(params.procSpec, delayBuffer.get());
        colony[i]->prepare(colonyParams);
    }   
}

void MicrobiomeEngine::processAudio(juce::AudioBuffer<float>& buffer)
{
    for (int i = 0; i < MAX_COLONY; i++) {
        colony[i]->toggleState(*parameters.getRawParameterValue(PARAMETER_ENABLE_ID(i + 1)) == 1.0f);

        if (colony[i]->isActive()) {
            colony[i]->processAudio(buffer);
        }
    }

    int delayWriteStart = delayWriteIdx;
    //int delayReadStart = delayReadIdx;
    for (int channel = 0; channel < buffer.getNumChannels() && channel < MAX_CHANNELS; channel++)
    {
        auto* channelData = buffer.getWritePointer (channel);
        delayWriteIdx = delayWriteStart;
        //delayReadIdx = delayReadStart;

        for (int i = 0; i < buffer.getNumSamples(); i++) {
            float originalSample = channelData[i];
            float colonySample = 0;
            for (int j = 0; j < MAX_COLONY; j++) {
                // can't break out early from checking appliedColonies because some colonies may be ramping down
                if (colony[j]->isActive()) {
                    colonySample += colony[j]->getSampleN(channel);
                }
            }

            float wet = *parameters.getRawParameterValue(PARAMETER_ENGINE_WET_ID);
            channelData[i] = ((1.0 - wet) * originalSample) + (colonySample * wet);

            // we update the delay after the current output sample has already been updated
            delayBuffer->setSample(channel, delayWriteIdx, originalSample);
            delayWriteIdx = (delayWriteIdx+1) % delayBuffer->getNumSamples();

            // DBG("Read Idx: " << delayReadIdx << "\tWrite Idx: " << delayWriteIdx);
            // DBG(delayBuffer->getSample(channel, delayReadIdx));
        }
    }

    reverbParameters.wetLevel = *parameters.getRawParameterValue(PARAMETER_ENGINE_REVERB_ID);
    reverbParameters.roomSize = *parameters.getRawParameterValue(PARAMETER_ENGINE_LUSH_ID);
    reverb.setParameters(reverbParameters);
    
    // set up local buffer effects chain processing
    juce::dsp::AudioBlock<float> localBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> procCtx(localBlock);

    reverb.process(procCtx);
}

void MicrobiomeEngine::disableColony(int n)
{
    if (colony[n]->isAlive()) {
        DBG("Disabling Colony: " << n);
        colony[n]->toggleState(false);
        //activeColonies--;
    }
}

void MicrobiomeEngine::enableColony(int n)
{
    if (!colony[n]->isAlive()) {
        DBG("Enabling Colony: " << n);
        colony[n]->toggleState(true);
        //activeColonies++;
    }
}

void MicrobiomeEngine::removeColony()
{
    for (int i = MAX_COLONY-1; i >= 0; i--) {
        if (colony[i]->isAlive()) {
            disableColony(i);
            break;
        }
    }
    //DBG("Active Colonies: " << activeColonies);
}

void MicrobiomeEngine::addColony()
{
    for (int i = 0; i < MAX_COLONY; i++) {
        if (!colony[i]->isAlive()) {
            enableColony(i);
            break;
        }
    }
    //DBG("Active Colonies: " << activeColonies);
}

void MicrobiomeEngine::setBPM(double bpm)
{
    currentBpm = bpm;
}

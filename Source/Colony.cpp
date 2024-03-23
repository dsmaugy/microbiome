/*
  ==============================================================================

    Colony.cpp
    Created: 18 Mar 2024 1:03:18pm
    Author:  darwin

  ==============================================================================
*/

#include "Colony.h"

#define GAIN_FADE_TIME 0.5
#define COLONY_BUFFER_LENGTH_SEC 5

Colony::Colony() 
{
}


void Colony::prepare(const ColonyParams& params)
{
    this->params = params;

    colonyBuffer = std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, params.procSpec.sampleRate * COLONY_BUFFER_LENGTH_SEC);
    std::stringstream bufferAddr;
    bufferAddr << std::hex << reinterpret_cast<std::uintptr_t>(colonyBuffer.get());
    DBG("Colony buffer created at: 0x" << bufferAddr.str());
    
    gain.reset(params.procSpec.sampleRate, GAIN_FADE_TIME);
    gain.setCurrentAndTargetValue(0);
    gain.setTargetValue(params.initialGain);

    resampleRatio = params.initialResampleRatio;

    // perform all channel-dependent init operations
    for (int i = 0; i < MAX_CHANNELS; i++) {
        resampler[i].reset();
        colonyBufferWriteIdx[i] = 0;
        colonyBufferReadIdx[i] = 0;
        resampleIdx[i] = 0;
    }
}

void Colony::processAudio(const juce::AudioBuffer<float>& buffer)
{
    // copy data to local colony buffer so we don't modify the original signal
    int numChannels = buffer.getNumChannels();
    int numInSamples = buffer.getNumSamples();
    for (int i = 0; i < numChannels; i++) {
        // pure copy
        //colonyBuffer->copyFrom(i, colonyBufferWriteIdx[i], buffer, i, 0, numInSamples);
        //colonyBufferWriteIdx[i] = (colonyBufferWriteIdx[i] + numInSamples) % (colonyBuffer->getNumSamples() - params.procSpec.maximumBlockSize);

        // resample copy
        auto numOutputSamples = numInSamples; // TODO: this could be an arbitrary number as long as its above block size

        //resampleIdx[i] = resampleIdx[i] % params.delayBuffer->getNumSamples();
        int used = resampler[i].process(resampleRatio, 
            params.delayBuffer->getReadPointer(i) + resampleStart + resampleIdx[i],
            colonyBuffer->getWritePointer(i) + colonyBufferWriteIdx[i], 
            numOutputSamples);
         resampleIdx[i] = (resampleIdx[i] + used) % (params.delayBuffer->getNumSamples() - params.procSpec.maximumBlockSize);
         colonyBufferWriteIdx[i] = (colonyBufferWriteIdx[i] + numOutputSamples) % (colonyBuffer->getNumSamples() - params.procSpec.maximumBlockSize);
        
        // DBG(used << "/" << buffer.getNumSamples() << " samples used");

        // resampleBuffer->clear(i, 0, resampleBuffer->getNumSamples());
        // resampleBuffer->copyFrom(i, 0)
    }

    // set up local buffer effects chain processing
    juce::dsp::AudioBlock<float> localBlock(*colonyBuffer);
    juce::dsp::ProcessContextReplacing<float> procCtx(localBlock);

    if (!gain.isSmoothing() && currentState == Colony::State::RAMP_DOWN) {
        // gain should be 0 here anyways
        DBG("Setting colony to DEAD");
        currentState = Colony::State::DEAD;
    }

    // if (rng.nextFloat() < 0.005) {
    //     DBG("Setting new random delay!");
    //     delayInSamples.setTargetValue(rng.nextInt(params.procSpec.sampleRate * 15) + params.procSpec.sampleRate);
    // }
    // delay.setDelay(delayInSamples.skip(buffer.getNumSamples()));
    // delay.process(procCtx);
}

// TODO: get rid of the n
// TODO: implement multichannel interleaving
float Colony::getSampleN(int channel, int n)
{
    float ret = colonyBuffer->getSample(channel, colonyBufferReadIdx[channel]) * gain.getNextValue();
    colonyBufferReadIdx[channel] = ((colonyBufferReadIdx[channel] + 1) % colonyBufferReadLength) + colonyBufferReadStart;

    return ret;
}

void Colony::toggleState(bool value)
{
    if (value && !isActive()) {
        currentState = Colony::State::ALIVE;
        gain.setTargetValue(params.initialGain);
    } else if (!value && isActive()) {
        currentState = Colony::State::RAMP_DOWN;
        DBG("Ramping down colony");
        gain.setTargetValue(0);
    }
}

bool Colony::isActive()
{
    return (currentState != Colony::State::DEAD);
}

bool Colony::isAlive()
{
    return (currentState == Colony::State::ALIVE);
}

float Colony::getGain()
{
    return gain.getCurrentValue();
}

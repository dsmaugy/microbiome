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
#define RESAMPLE_FADE 0.5

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

    loopFade.reset(params.procSpec.sampleRate, 0.15);
    loopFade.setCurrentAndTargetValue(0);

    resampleRatio.reset(params.procSpec.sampleRate, RESAMPLE_FADE);
    resampleRatio.setCurrentAndTargetValue(params.initialResampleRatio);

    ladder.setMode(juce::dsp::LadderFilterMode::LPF12);
    ladder.prepare(params.procSpec);
    ladder.setCutoffFrequencyHz(1000);

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
    int numOutputSamples = std::min(numInSamples, colonyBuffer->getNumSamples() - colonyBufferWriteIdx[0]); // TODO: this could be an arbitrary number as long as its above block size
    for (int i = 0; i < numChannels; i++) {
        // pure copy
        //colonyBuffer->copyFrom(i, colonyBufferWriteIdx[i], buffer, i, 0, numInSamples);
        //colonyBufferWriteIdx[i] = (colonyBufferWriteIdx[i] + numInSamples) % (colonyBuffer->getNumSamples() - params.procSpec.maximumBlockSize);

        // resample copy
        int sampleLimit = std::max(params.delayBuffer->getNumSamples()-1, resampleStart + resampleLength);

        int used = resampler[i].process(resampleRatio.getCurrentValue(),
            params.delayBuffer->getReadPointer(i) + resampleIdx[i],
            colonyBuffer->getWritePointer(i) + colonyBufferWriteIdx[i], 
            numOutputSamples, 
            sampleLimit - resampleIdx[i], 
            0);
        resampleIdx[i] += used;
        if (resampleIdx[i] >= sampleLimit) {
            resampleIdx[i] = resampleStart;
        }
        
        DBG(used << "/" << resampleLength << " samples used");
    }
    resampleRatio.skip(numInSamples);

    if (!gain.isSmoothing() && currentState == Colony::State::RAMP_DOWN) {
        // gain should be 0 here anyways
        DBG("Setting colony to DEAD");
        currentState = Colony::State::DEAD;
    }

    // set up local buffer effects chain processing
    juce::dsp::AudioBlock<float> localBlock(colonyBuffer->getArrayOfWritePointers(), colonyBuffer->getNumChannels(), colonyBufferWriteIdx[0], numOutputSamples);
    juce::dsp::ProcessContextReplacing<float> procCtx(localBlock);

    //ladder.process(procCtx);

    // if (rng.nextFloat() < 0.005) {
    //     DBG("Setting new random delay!");
    //     delayInSamples.setTargetValue(rng.nextInt(params.procSpec.sampleRate * 15) + params.procSpec.sampleRate);
    // }

    // update write indices
    for (int i = 0; i < MAX_CHANNELS; i++) {
        colonyBufferWriteIdx[i] = (colonyBufferWriteIdx[i] + numOutputSamples) % colonyBuffer->getNumSamples();
    }
}

// TODO: get rid of the n
float Colony::getSampleN(int channel, int n)
{
    // TODO: fade gain when approaching end/start of sample loop
    float ret = colonyBuffer->getSample(channel, colonyBufferReadIdx[channel]) * gain.getNextValue();// *loopFade.getNextValue();
    colonyBufferReadIdx[channel] = ((colonyBufferReadIdx[channel] + 1) % (colonyBufferReadLength + colonyBufferReadStart)) + colonyBufferReadStart;

    if (colonyBufferReadIdx[channel] == colonyBufferReadStart) {
        // playing from beginning of loop, need to fade in
        loopFade.setTargetValue(1);
    } else if (colonyBufferReadIdx[channel] == (colonyBufferReadLength + colonyBufferReadStart) * 0.85) {
        loopFade.setTargetValue(0);
    }
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

void Colony::setResampleRatio(float ratio)
{
    resampleRatio.setTargetValue(ratio);
}
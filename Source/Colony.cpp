/*
  ==============================================================================

    Colony.cpp
    Created: 18 Mar 2024 1:03:18pm
    Author:  darwin

  ==============================================================================
*/

#include "Colony.h"

#define GAIN_FADE_TIME 0.5
#define DELAY_FADE_TIME 1
#define MAX_DELAY_SEC 20

Colony::Colony() 
{
}


void Colony::prepare(const ColonyParams& params)
{
    this->params = params;

    colonyBuffer = std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, params.procSpec.maximumBlockSize);
    // TODO: might need to be 2x block size to fit all resample ratios
    resampleBuffer = std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, params.procSpec.maximumBlockSize);
    std::stringstream bufferAddr;
    bufferAddr << std::hex << reinterpret_cast<std::uintptr_t>(colonyBuffer.get());
    DBG("Colony buffer created at: 0x" << bufferAddr.str());
    
    // TODO: very inefficient here, could probably spend some time thinking of a way to optimize
    delayInSamples.reset(params.procSpec.sampleRate, DELAY_FADE_TIME);
    delayInSamples.setCurrentAndTargetValue(params.initialDelayInSamples);
    delay.setMaximumDelayInSamples(MAX_DELAY_SEC * params.procSpec.sampleRate);
    delay.setDelay(params.initialDelayInSamples);
    delay.prepare(params.procSpec);

    gain.reset(params.procSpec.sampleRate, GAIN_FADE_TIME);
    gain.setCurrentAndTargetValue(0);
    gain.setTargetValue(params.initialGain);

    // perform all channel-dependent init operations
    for (int i = 0; i < params.procSpec.numChannels; i++) {
        resampler[i].reset();
    }
    DBG("Colony delay created:" << "\tdelay (samples)=" << params.initialDelayInSamples << "\tdelay (sec)=" << (double) params.initialDelayInSamples/params.procSpec.sampleRate << " sec");
}

void Colony::processAudio(const juce::AudioBuffer<float>& buffer)
{
    // copy data to local colony buffer so we don't modify the original signal
    int numChannels = buffer.getNumChannels();
    int numInSamples = buffer.getNumSamples();
    for (int i = 0; i < numChannels; i++) {
        // pure copy
        // colonyBuffer->copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
        
        // resample copy
        auto readIn = buffer.getReadPointer(i);
        float ratio = 0.5;
        // take the input 
        resampleBuffer->copyFrom(i, resampleIndex, readIn, numInSamples-resampleIndex);
        int used = resampler[i].process(ratio, resampleBuffer->getReadPointer(i), colonyBuffer->getWritePointer(i), colonyBuffer->getNumSamples(), resampleBuffer->getNumSamples(), resampleBuffer->getNumSamples());
        DBG(used << "/" << buffer.getNumSamples() << " samples used");

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

float Colony::getSampleN(int channel, int n)
{
    // delay.setDelay(delayInSamples.getNextValue());
    delay.pushSample(channel, colonyBuffer->getSample(channel, n));
    return delay.popSample(channel, delayInSamples.getNextValue()) * gain.getNextValue();
    // return colonyBuffer->getSample(channel, n) * gain.getNextValue();
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

void Colony::setDelayTime(float sec)
{   
    // TODO: ramp down gain here to minimize audible delay modulation effect
    DBG("Setting delay to: " << sec << " seconds");
    delayInSamples.setTargetValue(sec * params.procSpec.sampleRate);
}
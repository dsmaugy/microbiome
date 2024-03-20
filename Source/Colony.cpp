/*
  ==============================================================================

    Colony.cpp
    Created: 18 Mar 2024 1:03:18pm
    Author:  darwin

  ==============================================================================
*/

#include "Colony.h"

#define FADE_TIME 0.5

Colony::Colony() 
{
}


void Colony::prepare(const ColonyParams& params)
{
    this->params = params;

    colonyBuffer = std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, params.procSpec.maximumBlockSize);
    std::stringstream bufferAddr;
    bufferAddr << std::hex << reinterpret_cast<std::uintptr_t>(colonyBuffer.get());
    DBG("Colony buffer created at: 0x" << bufferAddr.str());
    
    delay.setMaximumDelayInSamples(params.delayInSamples);
    delay.setDelay(params.delayInSamples);
    delay.prepare(params.procSpec);

    gain.reset(params.procSpec.sampleRate, FADE_TIME);
    gain.setCurrentAndTargetValue(0);
    gain.setTargetValue(params.initialGain);
    DBG("Colony delay created:" << "\tdelay (samples)=" << params.delayInSamples << "\tdelay (sec)=" << (double) params.delayInSamples/params.procSpec.sampleRate << " sec");
}

void Colony::processAudio(const juce::AudioBuffer<float>& buffer)
{
    // copy data to local colony buffer so we don't modify the original signal
    int numChannels = buffer.getNumChannels();
    for (int i = 0; i < numChannels; i++) {
        colonyBuffer->copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
    }

    // set up local buffer effects chain processing
    juce::dsp::AudioBlock<float> localBlock(*colonyBuffer);
    juce::dsp::ProcessContextReplacing<float> procCtx(localBlock);

    // DBG(gain.getNextValue());
    if (!gain.isSmoothing() && currentState == Colony::State::RAMP_DOWN) {
        // gain should be 0 here anyways
        DBG("Setting colony to DEAD");
        currentState = Colony::State::DEAD;
    }

    delay.process(procCtx);
}

float Colony::getSampleN(int channel, int n)
{
    return colonyBuffer->getSample(channel, n) * gain.getNextValue();
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
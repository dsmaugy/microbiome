/*
  ==============================================================================

    Colony.cpp
    Created: 18 Mar 2024 1:03:18pm
    Author:  darwin

  ==============================================================================
*/

#include "Colony.h"

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
    DBG("Colony delay created:" << "\tdelay (samples)=" << params.delayInSamples << "\tdelay (sec)=" << (double) params.delayInSamples/params.procSpec.sampleRate << " sec");
}

void Colony::processAudio(const juce::AudioBuffer<float>& buffer)
{
    int numChannels = buffer.getNumChannels();
    for (int i = 0; i < numChannels; i++) {
        colonyBuffer->copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
    }

    juce::dsp::AudioBlock<float> localBlock(*colonyBuffer);
    juce::dsp::ProcessContextReplacing<float> procCtx(localBlock);

    delay.process(procCtx);
}

float Colony::getSampleN(int channel, int n)
{
    return colonyBuffer->getSample(channel, n);
}

void Colony::setActive(bool value)
{
    active = value;
}

bool Colony::isActive()
{
    return active;
}

float Colony::getGain()
{
    return params.gain;
}
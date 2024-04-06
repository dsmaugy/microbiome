/*
  ==============================================================================

    GlobalDelay.cpp
    Created: 5 Apr 2024 11:20:33pm
    Author:  darwin

  ==============================================================================
*/

#include "GlobalDelay.h"
#include "Constants.h"

GlobalDelay::GlobalDelay(double sampleRate) 
    : buffer(std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, MAX_DELAY_SECONDS * sampleRate))
{
    // without clearing there are audible pops at initialization with using IIR filters
    buffer->clear();
}

void GlobalDelay::pushSample(int channel, float sample)
{
    buffer->setSample(channel, writeIdx, sample);
    writeIdx = (writeIdx + 1) % buffer->getNumSamples();
}

float* GlobalDelay::getNSamplesBehind(int channel, int n)
{
    int stepsToSkipFromOldest = (buffer->getNumSamples() - n);
    return buffer->getWritePointer(channel, (writeIdx + stepsToSkipFromOldest) % buffer->getNumSamples());
}

int GlobalDelay::getNumSamples() {
    return buffer->getNumSamples();
}
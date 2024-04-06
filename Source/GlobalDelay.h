/*
  ==============================================================================

    GlobalDelay.h
    Created: 5 Apr 2024 11:20:33pm
    Author:  darwin

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class GlobalDelay
{
public:
    GlobalDelay(double sampleRate);

    void pushSample(int channel, float sample);

    // returns a pointer to the sample N samples behind the most current sample
    float* getNSamplesBehind(int channel, int n);
    int getNumSamples();
private:
    // index of the oldest sample in delay
    int writeIdx = 0;
    std::unique_ptr<juce::AudioSampleBuffer> buffer;
};
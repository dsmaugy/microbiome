/*
  ==============================================================================

    Colony.h
    Created: 18 Mar 2024 1:03:18pm
    Author:  darwin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"

struct ColonyParams
{
    juce::dsp::ProcessSpec procSpec;
    int delayInSamples = 0;
    float gain = 0.5;
};

class Colony
{
    public:
        Colony();

        void prepare(const ColonyParams& params);
        float getSampleN(int channel, int n);

        void setActive(bool value);
        bool isActive();
        float getGain();

    private:
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay;
        std::unique_ptr<juce::AudioBuffer<float>> colonyBuffer;
        bool active;

        ColonyParams params;
};
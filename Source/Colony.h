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
    int blockSize;
};

class Colony
{
    public:
        Colony();

        void prepare(const ColonyParams& params);

        void setActive(bool value);
        bool getActiveStatus();

    private:
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay;
        std::unique_ptr<juce::AudioBuffer<float>> colonyBuffer;
        bool isActive;
};
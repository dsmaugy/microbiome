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
    float initialGain = 0.5;
};

class Colony
{
    public:
        Colony();

        void prepare(const ColonyParams& params);

        // copies buffer data into local colony buffer and applies colony processing on that local buffer
        void processAudio(const juce::AudioBuffer<float>& buffer);
        float getSampleN(int channel, int n);

        // set true to activate colony, false to kill it
        void toggleState(bool value);
        bool isActive();
        // colonies ramping down are not alive anymore
        bool isAlive();
        float getGain();

    private:
        enum class State
        {
            ALIVE,
            RAMP_UP,
            RAMP_DOWN,
            DEAD
        };
        Colony::State currentState = Colony::State::DEAD;

        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay;
        std::unique_ptr<juce::AudioBuffer<float>> colonyBuffer;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain;

        ColonyParams params;


};
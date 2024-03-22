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
    int initialDelayInSamples = 44100;
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

        void setDelayTime(float sec);
        float getGain();

    private:
        enum class State
        {
            ALIVE,
            RAMP_UP,
            RAMP_DOWN,
            RAMP_DOWN_MODIFY, // when the colony is tuning down its gain to modulate some parameters
            DEAD
        };
        Colony::State currentState = Colony::State::DEAD;
        
        // TODO: change this back to linear if its not doing anything
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delay;
        juce::Interpolators::Lagrange resampler[MAX_CHANNELS];
        std::unique_ptr<juce::AudioBuffer<float>> colonyBuffer;
        
        // holds leftover samples from downsampling techniques 
        std::unique_ptr<juce::AudioBuffer<float>> resampleBuffer;
        // index into the END of leftover samples section
        int resampleIndex = 0;

        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> delayInSamples;

        juce::Random rng;

        ColonyParams params;


};
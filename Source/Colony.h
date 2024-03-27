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
    // TODO: could prob make this a tiny more efficient by making this a reference but whatever
    juce::dsp::ProcessSpec procSpec;
    juce::AudioBuffer<float>* delayBuffer; // global delay buffer, should be only READING samples

    float initialGain = 0.5;
    float initialResampleRatio = 1.25;

    ColonyParams(juce::dsp::ProcessSpec spec, juce::AudioBuffer<float>* buffer) : procSpec(spec), delayBuffer(buffer) {}
    ColonyParams() {}
    // ColonyParams& operator=(const ColonyParams& other) {
    //     if (this != &other) {
    //         procSpec = other.procSpec;
    //         initialGain = other.initialGain;
    //         delayBuffer = other.delayBuffer;
    //     }
    //     return *this;
    // }
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

        void setResampleRatio(float ratio);
        void setColonyBufferReadStart(float startSec);
        void setColonyBufferReadLength(float lengthSec);
        void setResampleStart(float startSec);
        void setResampleLength(float lengthSec);

    private:
        enum class State
        {
            ALIVE,
            RAMP_UP,
            RAMP_DOWN,
            RAMP_DOWN_MODIFY, // when the colony is tuning down its gain to modulate some parameters
            DEAD
        };

        enum class ProcessMode
        {
            LOOP,
            REGENERATE
        };
        Colony::State currentState = Colony::State::DEAD;
        Colony::ProcessMode currentMode = Colony::ProcessMode::REGENERATE;
        std::unique_ptr<juce::AudioBuffer<float>> colonyBuffer;

        juce::Interpolators::Lagrange resampler[MAX_CHANNELS];
        juce::dsp::LadderFilter<float> ladder;

        bool doneProcessing = false;

        int colonyBufferWriteIdx[MAX_CHANNELS];
        int colonyBufferReadOffset[MAX_CHANNELS];
        int colonyBufferReadStart = 0;
        int colonyBufferReadLength = 132300;
        int colonyBufferReadOffsetLimit = 132300;
        
        int resampleIdx[MAX_CHANNELS];
        int resampleStart = 0;
        int resampleLength = 132300; // TODO: this doesn't really do anything
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> resampleRatio;

        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> loopFade;
        
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> delayInSamples;

        juce::Random rng;

        ColonyParams params;


};
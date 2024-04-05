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

class Colony : public juce::AudioProcessorValueTreeState::Listener
{
    public:
        Colony(int n, juce::AudioProcessorValueTreeState& parameters);

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
            RAMP_DOWN,
            DEAD
        };

        enum ProcessMode
        {
            LOOP,
            REGENERATE,
            FOLLOW
        };

        juce::AudioProcessorValueTreeState& parameters;
        int colonyNum;

        juce::String resampleRatioParamName;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> resampleRatio;

        Colony::State currentState = Colony::State::DEAD;
        Colony::ProcessMode currentMode = Colony::ProcessMode::LOOP;
        juce::String currentModeParamName;
        int currentModeIntRep = 0;

        std::unique_ptr<juce::AudioBuffer<float>> resampleBuffer;
        std::unique_ptr<juce::AudioBuffer<float>> outputBuffer;

       
        juce::Interpolators::Lagrange resampler[MAX_CHANNELS];

        juce::dsp::LadderFilter<float> ladder;
        juce::String ladderFreqParamName;
        int ladderFreq = 0;

        juce::dsp::Compressor<float> compressor;

        bool doneProcessing = false; // marks processing for resampling
        bool processEffects = false; // marks processing for effects with DSP modules
        int processEffectIndex = 0;

        juce::String colonyBufferReadStartParamName;
        int resampleBufferWriteIdx[MAX_CHANNELS];
        int colonyBufferReadOffset[MAX_CHANNELS];
        int colonyBufferReadStart = 0;
        juce::String colonyBufferReadEndParamName;
        int colonyBufferReadEnd = 132300;
        int colonyBufferReadOffsetLimit = 132300;

        // process cycle variables: these get updated every processBlock call
        int outReadCount[MAX_CHANNELS];
        int numResampledOutput = 0;

        juce::String ghostDelayParamName;
        int numGhosts = 0;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> ghostDelays[MAX_GHOSTS][MAX_CHANNELS];
        
        juce::String resampleStartParamName;
        int resampleIdx[MAX_CHANNELS];
        int resampleStart = 0;
        //int resampleLength = 132300; // TODO: this doesn't really do anything

        juce::String gainParamName;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> enableGain; // used for fading out colony when killing it
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> loopFade;
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> colonyLifeVol;
        
        juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> delayInSamples;

        juce::Random rng;

        ColonyParams params;

        void parameterChanged(const juce::String &parameterID, float newValue) override;
};
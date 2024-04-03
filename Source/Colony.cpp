/*
  ==============================================================================

    Colony.cpp
    Created: 18 Mar 2024 1:03:18pm
    Author:  darwin

  ==============================================================================
*/

#include "Colony.h"
#include "Constants.h"

#define GAIN_FADE_TIME 0.5
#define RESAMPLE_FADE 0.5
#define LOOP_FADE_TIME 0.4

#define COLONY_LIFE_THRESH 0.01

Colony::Colony(int n, juce::AudioProcessorValueTreeState& p) : colonyNum(n),
                        parameters(p),
                        colonyBuffer(std::make_unique<juce::AudioBuffer<float>>(0, 0)),
                        resampleRatioParamName(PARAMETER_RESAMPLE_RATIO_ID(n+1)), // for the users to see
                        resampleStartParamName(PARAMETER_RESAMPLE_START_ID(n+1)),
                        colonyBufferReadStartParamName(PARAMETER_COLONY_START_ID(n+1)),
                        colonyBufferReadEndParamName(PARAMETER_COLONY_END_ID(n+1)),
                        gainParamName(PARAMETER_COLONY_DBFS_ID(n+1)),
                        loopEnableParamName(PARAMETER_LOOP_ID(n+1)),
                        ghostDelayParamName(PARAMETER_COLONY_GHOST_ID(n+1))
{
}


void Colony::prepare(const ColonyParams& params)
{
    this->params = params;

    // TODO: prepare might get called more than once, we need to only reset things if sampleRate changed
    colonyBuffer = std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, params.procSpec.sampleRate * COLONY_BUFFER_LENGTH_SEC);
    colonyBuffer->clear();

    std::stringstream bufferAddr;
    bufferAddr << std::hex << reinterpret_cast<std::uintptr_t>(colonyBuffer.get());
    DBG("Colony buffer created at: 0x" << bufferAddr.str());
    
    gain.reset(params.procSpec.sampleRate, GAIN_FADE_TIME);
    gain.setCurrentAndTargetValue(0);
    gain.setTargetValue(params.initialGain);

    loopFade.reset(params.procSpec.sampleRate, LOOP_FADE_TIME);
    loopFade.setCurrentAndTargetValue(0);

    resampleRatio.reset(params.procSpec.sampleRate, RESAMPLE_FADE);
    resampleRatio.setCurrentAndTargetValue(params.initialResampleRatio);
    resampleLength = colonyBuffer->getNumSamples();

    ladder.setMode(juce::dsp::LadderFilterMode::LPF12);
    ladder.prepare(params.procSpec);
    ladder.setCutoffFrequencyHz(1000);

    compressor.prepare(params.procSpec);
    compressor.setAttack(600);
    compressor.setRelease(300);
    compressor.setThreshold(-5);
    compressor.setRatio(2);

    colonyLifeVol.reset(params.procSpec.sampleRate, 10);
    colonyLifeVol.setCurrentAndTargetValue(1);

    enableGain.reset(params.procSpec.sampleRate, GAIN_FADE_TIME);
    enableGain.setCurrentAndTargetValue(1);
    

    // perform all channel-dependent init operations
    for (int i = 0; i < MAX_CHANNELS; i++) {
        resampler[i].reset();
        colonyBufferWriteIdx[i] = 0;
        colonyBufferReadOffset[i] = 0;
        resampleIdx[i] = 0;

        for (int j = 0; j < MAX_GHOSTS; j++) {
            ghostDelays[j][i].reset(params.procSpec.sampleRate, 0.5);
            ghostDelays[j][i].setCurrentAndTargetValue(1000 + i*100);
        }
    }
}

void Colony::processAudio(const juce::AudioBuffer<float>& buffer)
{
    /****************************
    *  BEGIN PARAMETER PARSING  *
    *****************************/
    if (juce::approximatelyEqual(parameters.getRawParameterValue(loopEnableParamName)->load(), 0.0f)) {
        currentMode = Colony::ProcessMode::REGENERATE;
    }
    else {
        currentMode = Colony::ProcessMode::LOOP;
    }

    float newResampleRatio = *parameters.getRawParameterValue(resampleRatioParamName);
    if (!juce::approximatelyEqual(newResampleRatio, resampleRatio.getTargetValue())) {
        doneProcessing = false;
        resampleRatio.setTargetValue(newResampleRatio);
    }
    
    // handle new colony buffer read offset
    float newColonyBufferReadStartSec = *parameters.getRawParameterValue(colonyBufferReadStartParamName);    
    int newColonyBufferReadStart = (int) (newColonyBufferReadStartSec * params.procSpec.sampleRate);
    if (newColonyBufferReadStart != colonyBufferReadStart) {
        colonyBufferReadStart = newColonyBufferReadStart;
        colonyBufferReadStart = std::min(colonyBufferReadStart, colonyBuffer->getNumSamples() - 1); // clamp down start value
        //colonyBufferReadOffsetLimit = std::min(colonyBufferReadStart + colonyBufferReadLength, colonyBuffer->getNumSamples()) - colonyBufferReadStart;
        colonyBufferReadOffsetLimit = std::max(colonyBufferReadEnd - colonyBufferReadStart, 1);
        DBG("New Read Start Set: " << colonyBufferReadStart << " (sec= " << newColonyBufferReadStartSec << ")");
        loopFade.setCurrentAndTargetValue(0);

        for (int i = 0; i < MAX_CHANNELS; i++) {
            colonyBufferReadOffset[i] = 0;
        }   
    }

    // handle new colony buffer read end
    float newColonyBufferReadEndSec = *parameters.getRawParameterValue(colonyBufferReadEndParamName);
    int newColonyBufferReadEnd = (int)(newColonyBufferReadEndSec * params.procSpec.sampleRate);
    if (newColonyBufferReadEnd != colonyBufferReadEnd) {
        colonyBufferReadEnd = newColonyBufferReadEnd;
        colonyBufferReadOffsetLimit = std::max(colonyBufferReadEnd - colonyBufferReadStart, 1);
        DBG("New Read End Set: " << colonyBufferReadEnd << " (sec= " << newColonyBufferReadEndSec << ")");
    }
    
    // handle new starting point for pulling from main delay
    float newResampleStartSec = *parameters.getRawParameterValue(resampleStartParamName);
    int newResampleStart = (int) (newResampleStartSec * params.procSpec.sampleRate);
    if (newResampleStart != resampleStart) {
        resampleStart = newResampleStart;
        doneProcessing = false; 
        DBG("New Resample Start: " << resampleStart);
    }

    float newGain = juce::Decibels::decibelsToGain(parameters.getRawParameterValue(gainParamName)->load());
    if (!juce::approximatelyEqual(newGain, gain.getTargetValue())) {
        gain.setTargetValue(newGain);
        DBG("New Gain: " << newGain);
    }

    int newGhosts = (int) *parameters.getRawParameterValue(ghostDelayParamName);
    if (newGhosts != numGhosts) {
        numGhosts = newGhosts;
        DBG("New Ghosts: " << numGhosts);
    }

    /********************************************************
    *  END PARAMETER PARSING, BEGIN PROCESSING/STATE LOGIC  *
    *********************************************************/

    // update states
    if (!enableGain.isSmoothing() && currentState == Colony::State::RAMP_DOWN) {
        // gain should be 0 here anyways
        DBG("Setting colony to DEAD");
        currentState = Colony::State::DEAD;
    }

    // update ghost delays per processing block for efficiency
    for (int j = 0; j < numGhosts; j++) {
        if (!ghostDelays[j][0].isSmoothing() && rng.nextFloat() < 0.01) {
            int newGhost = rng.nextInt(4000);
            for (int i = 0; i < MAX_CHANNELS; i++) {
                ghostDelays[j][i].setTargetValue(newGhost);
            }
            //DBG("Re-generating ghost delay #" << j << " to " << newGhost);
        }
    }

    if (currentMode == Colony::ProcessMode::REGENERATE) {
        if (doneProcessing && colonyLifeVol.getCurrentValue() >= 0.95 && rng.nextFloat() < 0.01) {
            // begin process of killing colony
            colonyLifeVol.setTargetValue(COLONY_LIFE_THRESH);
            doneProcessing = false;
            DBG("Starting re-generation process");
        }
    }
    else if (currentMode == Colony::ProcessMode::LOOP) {
        // TODO: anything special here?
    }

    // transfer samples from main delay into colony buffer
    if (!doneProcessing) {
        int numChannels = buffer.getNumChannels();
        int numInSamples = buffer.getNumSamples();
        int numOutputSamples = std::min(numInSamples, colonyBuffer->getNumSamples() - colonyBufferWriteIdx[0]); // TODO: this could be an arbitrary number as long as its above block size
        int colonyBufferWriteProcStart = colonyBufferWriteIdx[0];
        for (int i = 0; i < numChannels; i++) {
            // resample copy
            int sampleLimit = std::max(params.delayBuffer->getNumSamples() - 1, resampleStart + resampleLength);

            int used = resampler[i].process(resampleRatio.getCurrentValue(),
                params.delayBuffer->getReadPointer(i) + resampleIdx[i],
                colonyBuffer->getWritePointer(i) + colonyBufferWriteIdx[i],
                numOutputSamples,
                sampleLimit - resampleIdx[i],
                0);
            resampleIdx[i] += used;
            if (resampleIdx[i] >= sampleLimit) {
                resampleIdx[i] = resampleStart;
            }

            //DBG(used << "/" << resampleLength << " samples used");
        }
        resampleRatio.skip(numInSamples);

        // set up local buffer effects chain processing
        juce::dsp::AudioBlock<float> localBlock(colonyBuffer->getArrayOfWritePointers(), colonyBuffer->getNumChannels(), colonyBufferWriteProcStart, numOutputSamples);
        juce::dsp::ProcessContextReplacing<float> procCtx(localBlock);

        ladder.process(procCtx);
        compressor.process(procCtx);
        // reverb.process(procCtx);

        // if (rng.nextFloat() < 0.005) {
        //     DBG("Setting new random delay!");
        //     delayInSamples.setTargetValue(rng.nextInt(params.procSpec.sampleRate * 15) + params.procSpec.sampleRate);
        // }

        // update write indices
        for (int i = 0; i < MAX_CHANNELS; i++) {
            colonyBufferWriteIdx[i] = (colonyBufferWriteIdx[i] + numOutputSamples) % colonyBuffer->getNumSamples();
        }

        if (colonyBufferWriteIdx[0] < colonyBufferWriteProcStart) {
            DBG("Done processing colony!");

            if (currentMode == Colony::ProcessMode::REGENERATE) {
                colonyLifeVol.setTargetValue(1);
                for (int i = 0; i < MAX_CHANNELS; i++) colonyBufferReadOffset[i] = 0;
            }
            doneProcessing = true;
        }
    }
}

// TODO: get rid of the n
float Colony::getSampleN(int channel, int n)
{
    //DBG("[CH= " << channel << "] Read Idx: " << colonyBufferReadOffset[channel] << "\tRead Limit: " << colonyBufferReadLimit << "\tRead Start: " << colonyBufferReadStart);

    // fade loops in/out
    if (colonyBufferReadOffset[channel] == 0) {
        loopFade.setTargetValue(1);
    }
    else if (colonyBufferReadOffset[channel] == (colonyBufferReadOffsetLimit * 0.75)) {
        loopFade.setTargetValue(0.35);
    }

    int sampleIndex = colonyBufferReadOffset[channel] + colonyBufferReadStart;
    float* sample = colonyBuffer->getWritePointer(channel, sampleIndex);
    float delaySample = 0;

    if (sampleIndex > 5000) {
        for (int i = 0; i < numGhosts; i++) {
            int delayInt = (int) ghostDelays[i][channel].getNextValue();
            float fr = ghostDelays[i][channel].getCurrentValue() - delayInt;
            float x0 = *(sample - delayInt);
            float x1 = *(sample - ((delayInt+1) % 5000));
            delaySample += ((1-fr)*x0 + fr*x1) * 0.50; // TODO: hardcode this number
            // delaySample = *(sample-1000);
        }
        //delaySample = colonyBuffer->getSample(channel, sampleIndex - 5000) + colonyBuffer->getSample(channel, sampleIndex - 4999) + colonyBuffer->getSample(channel, sampleIndex - 4998);
    }
    //colonyBuffer->setSample(channel, sampleIndex, sample * 0.4);
    colonyBufferReadOffset[channel] = ((colonyBufferReadOffset[channel] + 1) % colonyBufferReadOffsetLimit);
    //colonyBuffer->setSample(channel, sampleIndex, sample + delaySample * 0.4);
    return (*sample + delaySample) * gain.getNextValue() * loopFade.getNextValue() * enableGain.getNextValue() * colonyLifeVol.getNextValue();;
    //return ret;
}

void Colony::toggleState(bool value)
{
    if (value && !isActive()) {
        currentState = Colony::State::ALIVE;
        enableGain.setTargetValue(params.initialGain);
        doneProcessing = false; // if we revive this colony again it should start processing right away
        DBG("Reviving colony");
    } else if (!value && isActive()) {
        currentState = Colony::State::RAMP_DOWN;
        DBG("Ramping down colony");
        enableGain.setTargetValue(0);
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

void Colony::setResampleRatio(float ratio)
{
    resampleRatio.setTargetValue(ratio);
    doneProcessing = false;
}

// TODO: need a lock here???
void Colony::setColonyBufferReadStart(float startSec)
{
    //colonyBufferReadStart = (int)(startSec * params.procSpec.sampleRate);
    //colonyBufferReadStart = std::min(colonyBufferReadStart, colonyBuffer->getNumSamples() - 1); // clamp down start value
    //colonyBufferReadOffsetLimit = std::min(colonyBufferReadStart + colonyBufferReadLength, colonyBuffer->getNumSamples()) - colonyBufferReadStart;
    //DBG("New Read Start Set: " << colonyBufferReadStart << " (sec= " << startSec << ")");
    //loopFade.setCurrentAndTargetValue(0);

    //for (int i = 0; i < MAX_CHANNELS; i++) {
    //    colonyBufferReadOffset[i] = 0;
    //}
}

void Colony::setColonyBufferReadLength(float lengthSec)
{
    //colonyBufferReadLength = (int) (lengthSec * params.procSpec.sampleRate);
    //colonyBufferReadOffsetLimit = std::min(colonyBufferReadStart + colonyBufferReadLength, colonyBuffer->getNumSamples()) - colonyBufferReadStart;
    //DBG("New Read Limit: " << colonyBufferReadOffsetLimit << " (sec= " << lengthSec << ")");
    //doneProcessing = false;
}

// TODO: this prob has to be locked somehow
void Colony::setResampleStart(float startSec)
{
    resampleStart = (int) (startSec * params.procSpec.sampleRate);
    doneProcessing = false;
}

void Colony::setResampleLength(float length)
{

}

void Colony::parameterChanged(const juce::String &parameterID, float newValue) {
    // this should only run for enable toggles which should be pretty rare
    toggleState(newValue != 0.0f);
}

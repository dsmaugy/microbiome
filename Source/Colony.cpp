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

#define COLONY_LIFE_THRESH 0.05

Colony::Colony(int n) : colonyNum(n),
                        colonyBuffer(std::make_unique<juce::AudioBuffer<float>>(0, 0))
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

    reverb.prepare(params.procSpec);
    reverb.setParameters(reverbParameters);

    colonyLifeVol.reset(params.procSpec.sampleRate, 10);
    colonyLifeVol.setCurrentAndTargetValue(1);
    

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
    // update states
    if (!gain.isSmoothing() && currentState == Colony::State::RAMP_DOWN) {
        // gain should be 0 here anyways
        DBG("Setting colony to DEAD");
        currentState = Colony::State::DEAD;
    }

    // update ghost delays per processing block for efficiency
    for (int j = 0; j < MAX_GHOSTS; j++) {
        if (!ghostDelays[j][0].isSmoothing() && rng.nextFloat() < 0.01) {
            int newGhost = rng.nextInt(4000);
            for (int i = 0; i < MAX_CHANNELS; i++) {
                ghostDelays[j][i].setTargetValue(newGhost);
            }
            //DBG("Re-generating ghost delay #" << j << " to " << newGhost);
        }
    }

    if (currentMode == Colony::ProcessMode::REGENERATE) {
        if (doneProcessing && colonyLifeVol.getCurrentValue() <= COLONY_LIFE_THRESH+0.01) {
            // colony has faded out enough to be killed and replaced by new snippet
            doneProcessing = false;
            DBG("Re-generating colony");
        }
        else if (doneProcessing && colonyLifeVol.getCurrentValue() >= 0.95 && rng.nextFloat() < 0.01) {
            // begin process of killing colony
            colonyLifeVol.setTargetValue(COLONY_LIFE_THRESH);
            DBG("Starting re-generation process");
        }
    }

    if (!doneProcessing) {
        // copy data to local colony buffer so we don't modify the original signal
        int numChannels = buffer.getNumChannels();
        int numInSamples = buffer.getNumSamples();
        int numOutputSamples = std::min(numInSamples, colonyBuffer->getNumSamples() - colonyBufferWriteIdx[0]); // TODO: this could be an arbitrary number as long as its above block size
        int colonyBufferWriteProcStart = colonyBufferWriteIdx[0];
        for (int i = 0; i < numChannels; i++) {
            // pure copy
            //colonyBuffer->copyFrom(i, colonyBufferWriteIdx[i], buffer, i, 0, numInSamples);
            //colonyBufferWriteIdx[i] = (colonyBufferWriteIdx[i] + numInSamples) % (colonyBuffer->getNumSamples() - params.procSpec.maximumBlockSize);

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
        reverb.process(procCtx);

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
    //float ret = colonyBuffer->getSample(channel, colonyBufferReadOffset[channel] + colonyBufferReadStart) * gain.getNextValue() * loopFade.getNextValue();
    //colonyBuffer->applyGain(channel, n, 1, 0.5);
    int sampleIndex = colonyBufferReadOffset[channel] + colonyBufferReadStart;
    //float sample = colonyBuffer->getSample(channel, sampleIndex);
    float* sample = colonyBuffer->getWritePointer(channel, sampleIndex);
    float delaySample = 0;


    if (sampleIndex > 5000) {
        for (int i = 0; i < MAX_GHOSTS; i++) {
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
    return (*sample + delaySample) * gain.getNextValue() * loopFade.getNextValue() * colonyLifeVol.getNextValue();
    //return ret;
}

void Colony::toggleState(bool value)
{
    if (value && !isActive()) {
        currentState = Colony::State::ALIVE;
        gain.setTargetValue(params.initialGain);
        doneProcessing = false; // if we revive this colony again it should start processing right away
        DBG("Reviving colony");
    } else if (!value && isActive()) {
        currentState = Colony::State::RAMP_DOWN;
        DBG("Ramping down colony");
        gain.setTargetValue(0);
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
    colonyBufferReadStart = (int)(startSec * params.procSpec.sampleRate);
    colonyBufferReadStart = std::min(colonyBufferReadStart, colonyBuffer->getNumSamples() - 1); // clamp down start value
    colonyBufferReadOffsetLimit = std::min(colonyBufferReadStart + colonyBufferReadLength, colonyBuffer->getNumSamples()) - colonyBufferReadStart;
    DBG("New Read Start Set: " << colonyBufferReadStart << " (sec= " << startSec << ")");
    loopFade.setCurrentAndTargetValue(0);

    for (int i = 0; i < MAX_CHANNELS; i++) {
        colonyBufferReadOffset[i] = 0;
    }
}

void Colony::setColonyBufferReadLength(float lengthSec)
{
    colonyBufferReadLength = (int) (lengthSec * params.procSpec.sampleRate);
    colonyBufferReadOffsetLimit = std::min(colonyBufferReadStart + colonyBufferReadLength, colonyBuffer->getNumSamples()) - colonyBufferReadStart;
    DBG("New Read Limit: " << colonyBufferReadOffsetLimit << " (sec= " << lengthSec << ")");
    doneProcessing = false;
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

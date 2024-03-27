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

Colony::Colony() 
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

    // perform all channel-dependent init operations
    for (int i = 0; i < MAX_CHANNELS; i++) {
        resampler[i].reset();
        colonyBufferWriteIdx[i] = 0;
        colonyBufferReadOffset[i] = 0;
        resampleIdx[i] = 0;
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

            DBG(used << "/" << resampleLength << " samples used");
        }
        resampleRatio.skip(numInSamples);

        // set up local buffer effects chain processing
        juce::dsp::AudioBlock<float> localBlock(colonyBuffer->getArrayOfWritePointers(), colonyBuffer->getNumChannels(), colonyBufferWriteProcStart, numOutputSamples);
        juce::dsp::ProcessContextReplacing<float> procCtx(localBlock);

        ladder.process(procCtx);

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
            doneProcessing = true;
        }
    }
}

// TODO: get rid of the n
float Colony::getSampleN(int channel, int n)
{
    // fade loops in/out
    if (colonyBufferReadOffset[channel] == 0) {
        loopFade.setTargetValue(1);
    }
    else if (colonyBufferReadOffset[channel] == (colonyBufferReadOffsetLimit * 0.75)) {
        loopFade.setTargetValue(0.35);
    }

    //DBG("[CH= " << channel << "] Read Idx: " << colonyBufferReadOffset[channel] << "\tRead Limit: " << colonyBufferReadLimit << "\tRead Start: " << colonyBufferReadStart);
    float ret = colonyBuffer->getSample(channel, colonyBufferReadOffset[channel] + colonyBufferReadStart) * gain.getNextValue() * loopFade.getNextValue();
    colonyBufferReadOffset[channel] = ((colonyBufferReadOffset[channel] + 1) % colonyBufferReadOffsetLimit);

    return ret;
}

void Colony::toggleState(bool value)
{
    if (value && !isActive()) {
        currentState = Colony::State::ALIVE;
        gain.setTargetValue(params.initialGain);
        doneProcessing = false; // if we revive this colony again it should start processing right away
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
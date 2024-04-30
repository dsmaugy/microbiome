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
                        resampleBuffer(std::make_unique<juce::AudioBuffer<float>>(0, 0)),
                        resampleRatioParamName(PARAMETER_RESAMPLE_RATIO_ID(n+1)), // for the users to see
                        resampleStartParamName(PARAMETER_RESAMPLE_START_ID(n+1)),
                        colonyBufferReadStartParamName(PARAMETER_COLONY_START_ID(n+1)),
                        colonyBufferReadEndParamName(PARAMETER_COLONY_END_ID(n+1)),
                        gainParamName(PARAMETER_COLONY_DBFS_ID(n+1)),
                        ghostDelayParamName(PARAMETER_COLONY_GHOST_ID(n+1)),
                        ladderFreqParamName(PARAMETER_COLONY_FILTER_ID(n+1)),
                        currentModeParamName(PARAMETER_COLONY_MODE_ID(n+1)),
                        enabledParamName(PARAMETER_ENABLE_ID(n+1))
{
}


void Colony::prepare(const ColonyParams& params)
{
    this->params = params;

    if (!resampleBuffer || resampleBuffer->getNumSamples() != COLONY_BUFFER_LENGTH_SEC * (int) params.procSpec.sampleRate) {    resampleBuffer = std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, (int) params.procSpec.sampleRate * COLONY_BUFFER_LENGTH_SEC);
        resampleBuffer->clear();

        outputBuffer = std::make_unique<juce::AudioBuffer<float>>(MAX_CHANNELS, (int) params.procSpec.sampleRate * COLONY_BUFFER_LENGTH_SEC);
        outputBuffer->clear();

        std::stringstream bufferAddr;
        bufferAddr << std::hex << reinterpret_cast<std::uintptr_t>(resampleBuffer.get());
        DBG("Colony buffer created at: 0x" << bufferAddr.str());
    }

    
    gain.reset(params.procSpec.sampleRate, GAIN_FADE_TIME);
    gain.setCurrentAndTargetValue(0);
    gain.setTargetValue(params.initialGain);

    loopFade.reset(params.procSpec.sampleRate, LOOP_FADE_TIME);
    loopFade.setCurrentAndTargetValue(0);

    resampleRatio.reset(params.procSpec.sampleRate, RESAMPLE_FADE);
    resampleRatio.setCurrentAndTargetValue(params.initialResampleRatio);

    ladder.setMode(juce::dsp::LadderFilterMode::LPF12);
    ladder.prepare(params.procSpec);
    ladder.setCutoffFrequencyHz(1000);
    ladder.setResonance(0.2);

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
        resampleBufferWriteIdx[i] = 0;
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
    int numChannels = buffer.getNumChannels();
    int numInSamples = buffer.getNumSamples();
    /****************************
    *  BEGIN PARAMETER PARSING  *
    *****************************/
    // DBG("Current Mode: " << *parameters.getRawParameterValue(currentModeParamName));
    int newModeIntRep = (int) *parameters.getRawParameterValue(currentModeParamName);
    if (newModeIntRep != static_cast<int>(currentMode)) {
        currentMode = static_cast<Colony::ProcessMode>(newModeIntRep);
        DBG("New Mode: " << currentMode);
        doneProcessing = false;
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
        colonyBufferReadStart = std::min(colonyBufferReadStart, resampleBuffer->getNumSamples() - 1); // clamp down start value
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
    
    int newLadderFreq = (int) *parameters.getRawParameterValue(ladderFreqParamName);
    if (ladderFreq != newLadderFreq) {
        ladderFreq = newLadderFreq;
        ladder.setCutoffFrequencyHz(ladderFreq);
        processEffects = true;
    }

    float newEnabledParamValue = *parameters.getRawParameterValue(enabledParamName);
    if (newEnabledParamValue != enabledParamValue) {
        enabledParamValue = newEnabledParamValue;
        toggleState(enabledParamValue != 0.0f);
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
    else  {
        colonyLifeVol.setCurrentAndTargetValue(1.0);
    }

    // PROCESS STEP #0: reset counter
    for (int i = 0; i < MAX_CHANNELS; i++) outReadCount[i] = 0;

    // PROCESS STEP #1: resample audio
    numResampledOutput = -1;
    int prevWriteBuffStart = -1;
    if (!doneProcessing) {
        numResampledOutput = std::min(numInSamples, resampleBuffer->getNumSamples() - resampleBufferWriteIdx[0]); // TODO: this could be an arbitrary number as long as its above block size
        prevWriteBuffStart = resampleBufferWriteIdx[0]; // store the old value so we can reset it per channel
        for (int i = 0; i < MAX_CHANNELS; i++) {
            // resample copy
            //int sampleLimit = std::max(params.delayBuffer->getNumSamples() - 1, resampleStart + resampleLength);
            //int sampleLimit = 
            int used = resampler[i].process(resampleRatio.getCurrentValue(),
                params.delayBuffer->getReadPointer(i) + resampleIdx[i],
                resampleBuffer->getWritePointer(i) + resampleBufferWriteIdx[i],
                numResampledOutput,
                params.delayBuffer->getNumSamples() - resampleIdx[i],
                0);
            resampleIdx[i] += used;
            if (resampleIdx[i] >= params.delayBuffer->getNumSamples()) {
                resampleIdx[i] = resampleStart;
            }

            // DBG(used << " - " << resampleIdx[i] << " samples used\t(output=" << numResampledOutput << ")\tWriting at: " << resampleBufferWriteIdx[i] << "\tReading at : " << colonyBufferReadOffset[i]);
        }
        resampleRatio.skip(numInSamples);
        processEffects = true;

        // if (rng.nextFloat() < 0.005) {
        //     DBG("Setting new random delay!");
        //     delayInSamples.setTargetValue(rng.nextInt(params.procSpec.sampleRate * 15) + params.procSpec.sampleRate);
        // }

        // update write indices
        for (int i = 0; i < MAX_CHANNELS; i++) {
            resampleBufferWriteIdx[i] = (resampleBufferWriteIdx[i] + numResampledOutput) % resampleBuffer->getNumSamples();
        }

        if (resampleBufferWriteIdx[0] < prevWriteBuffStart) {
            DBG("Done processing resampling! RMS: " << resampleBuffer->getRMSLevel(0, 0, resampleBuffer->getNumSamples()) << "\tResample Write: " << resampleBufferWriteIdx[0] << "\tDelay Progress: " << resampleIdx[0] << "/" << params.delayBuffer->getNumSamples());

            if (currentMode == Colony::ProcessMode::REGENERATE) {
                colonyLifeVol.setTargetValue(1);
                for (int i = 0; i < MAX_CHANNELS; i++) colonyBufferReadOffset[i] = 0;
            }

            if (currentMode != Colony::ProcessMode::FOLLOW) doneProcessing = true;
        }
    }
    
    // PROCESS STEP #2: apply effects
    if (processEffects) {
        int startSampleIndex;
        int numberOfSamples;

        if (numResampledOutput > 0) {
            startSampleIndex = prevWriteBuffStart;
            numberOfSamples = numResampledOutput;
        }
        else {
            startSampleIndex = processEffectIndex;
            //startSampleIndex = colonyBufferReadOffset[0];
            numberOfSamples = std::min(numInSamples, outputBuffer->getNumSamples() - startSampleIndex);
        }
        //numberOfSamples = std::min(numInSamples, outputBuffer->getNumSamples() - startSampleIndex);
        //DBG("Processing effect -> reading diff: " << startSampleIndex - colonyBufferReadOffset[0] + colonyBufferReadStart);
        juce::dsp::AudioBlock<float> outputBlock(outputBuffer->getArrayOfWritePointers(), outputBuffer->getNumChannels(), startSampleIndex, numberOfSamples);
        // process replace on a duplicate block as to allow multiple process operations on a single block
        // destination offset starts at 0 since the block starts at the correct index in outputBuffer
        outputBlock.copyFrom(*resampleBuffer, startSampleIndex, 0, numberOfSamples); 
        juce::dsp::ProcessContextReplacing<float> procCtx(outputBlock);

        ladder.process(procCtx);
        compressor.process(procCtx);

        if (numResampledOutput < 0) {
            // manually update the process effect index if effect processing was triggered WITHOUT a resample processing trigger
            processEffectIndex = (processEffectIndex + numberOfSamples) % outputBuffer->getNumSamples();
        }

        if (startSampleIndex + numberOfSamples >= outputBuffer->getNumSamples()) {
            processEffects = false;
            DBG("Done processing effects!");
        }
    }
}

float Colony::getSampleN(int channel)
{
    float output = 0.0;
    if (numResampledOutput < 0 || (numResampledOutput > 0 && outReadCount[channel] < numResampledOutput)) {
        // fade loops in/out
        if (colonyBufferReadOffset[channel] == 0) {
            loopFade.setTargetValue(1);
        }
        else if (colonyBufferReadOffset[channel] == (colonyBufferReadOffsetLimit * 0.75)) {
            loopFade.setTargetValue(0.35);
        }

        int sampleIndex = colonyBufferReadOffset[channel] + colonyBufferReadStart;
        float* sample = outputBuffer->getWritePointer(channel, sampleIndex);
        float delaySample = 0;

        if (sampleIndex > 5000) {
            for (int i = 0; i < numGhosts; i++) {
                int delayInt = (int)ghostDelays[i][channel].getNextValue();
                float fr = ghostDelays[i][channel].getCurrentValue() - delayInt;
                float x0 = *(sample - delayInt);
                float x1 = *(sample - ((delayInt + 1) % 5000));
                delaySample += ((1 - fr) * x0 + fr * x1) * 0.50; // TODO: hardcode this number
                // delaySample = *(sample-1000);
            }
            //delaySample = colonyBuffer->getSample(channel, sampleIndex - 5000) + colonyBuffer->getSample(channel, sampleIndex - 4999) + colonyBuffer->getSample(channel, sampleIndex - 4998);
        }
        colonyBufferReadOffset[channel] = ((colonyBufferReadOffset[channel] + 1) % colonyBufferReadOffsetLimit);
        output = (*sample + delaySample) * gain.getNextValue() * loopFade.getNextValue() * enableGain.getNextValue() * colonyLifeVol.getNextValue();
        outReadCount[channel]++;
    }

    return output;
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


/*
  ==============================================================================

    Constants.h
    Created: 18 Mar 2024 1:09:05pm
    Author:  darwin

  ==============================================================================
*/

#pragma once


#define MAX_COLONY 4
#define DEFAULT_COLONIES 1
#define MAX_CHANNELS 2
#define MAX_DELAY_SECONDS 10 // global delay line length
#define COLONY_BUFFER_LENGTH_SEC 5
#define MAX_GHOSTS 20 // TODO: play around with this value

// audio parameter limits
#define RESAMPLE_RATIO_MAX 1.5
#define RESAMPLE_RATIO_MIN 0.5
#define RESAMPLE_RATIO_DEF 1
#define RESAMPLE_START_MAX (MAX_DELAY_SECONDS - COLONY_BUFFER_LENGTH_SEC)
#define RESAMPLE_START_MIN 0
#define RESAMPLE_START_DEF 0
#define COLONY_START_MAX (COLONY_BUFFER_LENGTH_SEC)
#define COLONY_START_MIN 0
#define COLONY_START_DEF 0

#define PARAMETER_ENABLE_ID(n) "colony_" + juce::String(n) + "_enable"
#define PARAMETER_ENABLE_NAME(n) "Colony " + juce::String(n) + " Enable"
#define PARAMETER_RESAMPLE_RATIO_ID(n) "colony_" + juce::String(n) + "_warp"
#define PARAMETER_RESAMPLE_RATIO_NAME(n) "Colony " + juce::String(n) + " Warp"
#define PARAMETER_RESAMPLE_START_ID(n) "colony_" + juce::String(n) + "_branch_start"
#define PARAMETER_RESAMPLE_START_NAME(n) "Colony " + juce::String(n) + " Branch Start"
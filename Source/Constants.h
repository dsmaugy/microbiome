/*
  ==============================================================================

    Constants.h
    Created: 18 Mar 2024 1:09:05pm
    Author:  darwin

  ==============================================================================
*/

#pragma once


#define MAX_COLONY 8
#define DEFAULT_COLONIES 1
#define MAX_CHANNELS 2
#define MAX_DELAY_SECONDS 10
#define COLONY_BUFFER_LENGTH_SEC 5
#define MAX_GHOSTS 20 // TODO: play around with this value

// audio parameter limits
#define RESAMPLE_RATIO_MAX 1.5
#define RESAMPLE_RATIO_MIN 0.5
#define RESAMPLE_RATIO_DEFAULT 1

#define PARAMETER_ENABLE_ID(n) "colony_" + juce::String(n) + "_enable"
#define PARAMETER_ENABLE_NAME(n) "Colony " + juce::String(n) + " Enable"
#define PARAMETER_RESAMPLE_RATIO_ID(n) "colony_" + juce::String(n) + "_warp"
#define PARAMETER_RESAMPLE_RATIO_NAME(n) "Colony " + juce::String(n) + " Warp"
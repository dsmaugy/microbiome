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

#define PARAMETER_ENABLE_ID(n) "colony_" + juce::String(n) + "_enable"
#define PARAMETER_ENABLE_NAME(n) "Colony " + juce::String(n) + " Enable"
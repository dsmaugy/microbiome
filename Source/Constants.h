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
#define MAX_GHOSTS 15 // TODO: play around with this value

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
#define COLONY_END_MAX (COLONY_BUFFER_LENGTH_SEC)
#define COLONY_END_MIN 0
#define COLONY_END_DEF (COLONY_BUFFER_LENGTH_SEC)
#define COLONY_DBFS_MIN -12
#define COLONY_DBFS_MAX 12
#define COLONY_DBFS_DEF -3
#define ENGINE_WET_MIN 0
#define ENGINE_WET_MAX 1
#define ENGINE_WET_DEF 0.5
#define COLONY_LOOP_DEF false
#define COLONY_GHOSTS_MAX (MAX_GHOSTS)
#define COLONY_GHOSTS_MIN 0
#define COLONY_GHOSTS_DEF 5

#define PARAMETER_ENABLE_ID(n) "colony_" + juce::String(n) + "_enable"
#define PARAMETER_ENABLE_NAME(n) "Colony " + juce::String(n) + " Enable"
#define PARAMETER_RESAMPLE_RATIO_ID(n) "colony_" + juce::String(n) + "_warp"
#define PARAMETER_RESAMPLE_RATIO_NAME(n) "Colony " + juce::String(n) + " Warp"
#define PARAMETER_RESAMPLE_START_ID(n) "colony_" + juce::String(n) + "_branch_start"
#define PARAMETER_RESAMPLE_START_NAME(n) "Colony " + juce::String(n) + " Branch Start"
#define PARAMETER_COLONY_START_ID(n) "colony_" + juce::String(n) + "_branch_loop_start"
#define PARAMETER_COLONY_START_NAME(n) "Colony " + juce::String(n) + " Branch Loop Start"
#define PARAMETER_COLONY_END_ID(n) "colony_" + juce::String(n) + "_branch_loop_end"
#define PARAMETER_COLONY_END_NAME(n) "Colony " + juce::String(n) + " Branch Loop End"
#define PARAMETER_COLONY_DBFS_ID(n) "colony_" + juce::String(n) + "_gain"
#define PARAMETER_COLONY_DBFS_NAME(n) "Colony " + juce::String(n) + " Gain"
#define PARAMETER_ENGINE_WET_ID "engine_wet"
#define PARAMETER_ENGINE_WET_NAME "Engine Wetness"
#define PARAMETER_LOOP_ID(n) "colony_" + juce::String(n) + "_loop"
#define PARAMETER_LOOP_NAME(n) "Colony " + juce::String(n) + " Loop"
#define PARAMETER_COLONY_GHOST_ID(n) "colony_" + juce::String(n) + "_ghost"
#define PARAMETER_COLONY_GHOST_NAME(n) "Colony " + juce::String(n) + " Artifacts"
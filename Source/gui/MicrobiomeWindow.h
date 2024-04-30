/*
  ==============================================================================

    MicrobiomeWindow.h
    Created: 3 Apr 2024 1:33:10am
    Author:  darwin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "../Constants.h"
#define NUM_NODE_LINES 15
#define NUM_TENTACLES 30


//==============================================================================
/*
*/
class MicrobiomeWindow :    public juce::AnimatedAppComponent

{
    struct DrawnTentacle {
        juce::Path tentacle;
        juce::AffineTransform transform;
    };

public:
    MicrobiomeWindow(MicrobiomeAudioProcessor& p, juce::AudioProcessorValueTreeState&);
    ~MicrobiomeWindow() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update() override;	

private:

    juce::AudioProcessorValueTreeState& parameters;
    MicrobiomeAudioProcessor& processor;
    float nodeLineShifts[NUM_NODE_LINES];
    DrawnTentacle tentacles[NUM_TENTACLES];
    float playbackIndicatorPos[MAX_COLONY];
    float playbackIndicatorDelta;

    juce::Random rng;
    

    DrawnTentacle generateTentacle();
    juce::Path generateColonyBranch();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MicrobiomeWindow)
};

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../PluginProcessor.h"
#include "TwoValueSliderAttachment.h"
#include "MicrobiomeWindow.h"
#include "MicrobiomeControls.h"
#include "MicrobiomeLAF.h"


//==============================================================================
/**
*/
class MicrobiomeAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                        private juce::Button::Listener,
                                        private juce::AudioProcessorValueTreeState::Listener
{
public:
    MicrobiomeAudioProcessorEditor (MicrobiomeAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~MicrobiomeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MicrobiomeAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& parameters;
    int currentColony = 0;

    MicrobiomeLAF laf;

    juce::Rectangle<int> p_topbarRect;
    juce::Rectangle<int> p_colMenuRect;

    MicrobiomeWindow visualWindow;
    MicrobiomeControls controlWindow;

    juce::TextButton nextColony = juce::TextButton("->");
    juce::TextButton prevColony = juce::TextButton("<-");

    void buttonClicked(juce::Button* button) override;
    void applyRotarySliderStyle(juce::Slider& slider);
    void parameterChanged(const juce::String &parameterID, float newValue) override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrobiomeAudioProcessorEditor)
};

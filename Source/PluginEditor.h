/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TwoValueSliderAttachment.h"
#include "MicrobiomeWindow.h"
#include "MicrobiomeControls.h"


//==============================================================================
/**
*/
class MicrobiomeAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    MicrobiomeAudioProcessorEditor (MicrobiomeAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~MicrobiomeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void applyRotarySliderStyle(juce::Slider& slider);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MicrobiomeAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& parameters;
    // int currentColony;

    MicrobiomeWindow visualWindow;
    MicrobiomeControls controlWindow;

    // juce::Slider resampleRatio;
    // juce::Slider colonyBufferReadLength;
    // juce::Slider colonyBufferReadStart;
    juce::TextButton addColony = juce::TextButton("->");
    juce::TextButton removeColony = juce::TextButton("<-");



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrobiomeAudioProcessorEditor)
};

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MicrobiomeAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::Slider::Listener, private juce::Button::Listener
{
public:
    MicrobiomeAudioProcessorEditor (MicrobiomeAudioProcessor&);
    ~MicrobiomeAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MicrobiomeAudioProcessor& audioProcessor;

    juce::Slider reverbWet;
    juce::TextButton addColony = juce::TextButton("Add Colony");
    juce::TextButton removeColony = juce::TextButton("Remove Colony");

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrobiomeAudioProcessorEditor)
};

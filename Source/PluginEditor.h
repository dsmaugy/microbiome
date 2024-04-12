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
    MicrobiomeAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& parameters;
    // int currentColony;

    juce::Rectangle<int> p_topbarRect;

    MicrobiomeWindow visualWindow;
    MicrobiomeControls controlWindow;

    juce::TextButton addColony = juce::TextButton("->");
    juce::TextButton removeColony = juce::TextButton("<-");

    void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void applyRotarySliderStyle(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrobiomeAudioProcessorEditor)
};

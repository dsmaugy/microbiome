/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

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

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MicrobiomeAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& parameters;
    int currentColony;

    juce::Slider resampleRatio;
    juce::Slider colonyBufferReadLength;
    juce::Slider colonyBufferReadStart;
    juce::TextButton addColony = juce::TextButton("->");
    juce::TextButton removeColony = juce::TextButton("<-");

    std::array<std::unique_ptr<juce::ToggleButton>, MAX_COLONY> enableColonyButtons;
    std::array<std::unique_ptr<ButtonAttachment>, MAX_COLONY> enableAttachments;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> resampleRatioSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> resampleRatioAttachments;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> resampleStartSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> resampleStartAttachments;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrobiomeAudioProcessorEditor)
};

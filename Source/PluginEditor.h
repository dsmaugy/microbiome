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
    void applyRotarySliderStyle(juce::Slider& slider);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MicrobiomeAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& parameters;
    int currentColony;

    MicrobiomeWindow visualWindow;

    juce::Slider resampleRatio;
    juce::Slider colonyBufferReadLength;
    juce::Slider colonyBufferReadStart;
    juce::TextButton addColony = juce::TextButton("->");
    juce::TextButton removeColony = juce::TextButton("<-");

    std::unique_ptr<juce::Slider> engineWetSlider;
    std::unique_ptr<SliderAttachment> engineWetAttachment;
    std::unique_ptr<juce::Slider> engineReverbSlider;
    std::unique_ptr<SliderAttachment> engineReverbAttachment;
    std::unique_ptr<juce::Slider> engineLushSlider;
    std::unique_ptr<SliderAttachment> engineLushAttachment;

    std::array<std::vector<juce::Component*>, MAX_COLONY> colonyComponents;
    std::array<std::unique_ptr<juce::ToggleButton>, MAX_COLONY> enableColonyButtons;
    std::array<std::unique_ptr<ButtonAttachment>, MAX_COLONY> enableAttachments;
    std::array<std::unique_ptr<juce::ToggleButton>, MAX_COLONY> loopColonyButtons;
    std::array<std::unique_ptr<ButtonAttachment>, MAX_COLONY> loopAttachments;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> resampleRatioSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> resampleRatioAttachments;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> resampleStartSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> resampleStartAttachments;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> colonyPlayControlSliders;
    std::array<std::unique_ptr<juce::TwoValueSliderAttachment>, MAX_COLONY> colonyPlayControlAttachments;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> colonyGainSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> colonyGainAttachments;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> colonyGhostSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> colonyGhostAttachments;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> colonyFilterSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> colonyFilterAttachments;
    std::array<std::unique_ptr<juce::ComboBox>, MAX_COLONY> colonyModeBox;
    std::array<std::unique_ptr<juce::ComboBoxParameterAttachment>, MAX_COLONY> colonyModeBoxAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrobiomeAudioProcessorEditor)
};

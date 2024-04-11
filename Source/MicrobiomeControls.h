/*
  ==============================================================================

    MicrobiomeControls.h
    Created: 6 Apr 2024 11:13:48am
    Author:  darwin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Constants.h"
#include "TwoValueSliderAttachment.h"
#include "MoveableLabel.h"

#include "LookAndFeels/ModeComboBoxLAF.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

//==============================================================================
/*
*/
class MicrobiomeControls  : public juce::Component
{
public:
    MicrobiomeControls(juce::AudioProcessorValueTreeState& apvst);
    ~MicrobiomeControls() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void incrementColonyView();
    void decrementColonyView();

private:
    int currentColony = 0;
    juce::AudioProcessorValueTreeState& parameters;

    // look and feels
    ModeComboBoxLAF cblaf;

    std::unique_ptr<juce::Slider> engineWetSlider;
    std::unique_ptr<SliderAttachment> engineWetAttachment;
    MoveableLabel engineWetLabel;
    std::unique_ptr<juce::Slider> engineReverbSlider;
    std::unique_ptr<SliderAttachment> engineReverbAttachment;
    MoveableLabel engineReverbLabel;
    std::unique_ptr<juce::Slider> engineLushSlider;
    std::unique_ptr<SliderAttachment> engineLushAttachment;
    MoveableLabel engineLushLabel;

    juce::Rectangle<int> engineCtrlsRect;
    juce::Rectangle<int> engineCtrlsTtlRect;

    std::array<std::vector<juce::Component*>, MAX_COLONY> colonyComponents;
    std::array<std::unique_ptr<juce::ToggleButton>, MAX_COLONY> enableColonyButtons;
    std::array<std::unique_ptr<ButtonAttachment>, MAX_COLONY> enableAttachments;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> resampleRatioSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> resampleRatioAttachments;
    MoveableLabel resampleRatioLabel;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> resampleStartSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> resampleStartAttachments;
    MoveableLabel resampleStartLabel;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> colonyPlayControlSliders;
    std::array<std::unique_ptr<juce::TwoValueSliderAttachment>, MAX_COLONY> colonyPlayControlAttachments;
    MoveableLabel playControlLabelTop;
    MoveableLabel playControlLabelBottom;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> colonyGainSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> colonyGainAttachments;
    MoveableLabel gainLabel;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> colonyGhostSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> colonyGhostAttachments;
    MoveableLabel ghostLabel;
    std::array<std::unique_ptr<juce::Slider>, MAX_COLONY> colonyFilterSliders;
    std::array<std::unique_ptr<SliderAttachment>, MAX_COLONY> colonyFilterAttachments;
    MoveableLabel lpfLabel;
    std::array<std::unique_ptr<juce::ComboBox>, MAX_COLONY> colonyModeBox;
    std::array<std::unique_ptr<juce::ComboBoxParameterAttachment>, MAX_COLONY> colonyModeBoxAttachment;
    MoveableLabel modeLabel;

    void applyRotarySliderStyle(juce::Slider& slider);
    void applyLinearSliderStyle(juce::Slider& slider);
    void setViewToColony(int n);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MicrobiomeControls)
};

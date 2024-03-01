/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MicrobiomeAudioProcessorEditor::MicrobiomeAudioProcessorEditor (MicrobiomeAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    reverbWet.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    reverbWet.setRange(0.0, 1.0, 0.1);
    reverbWet.setValue(0.5);
    reverbWet.setRotaryParameters(3 * juce::MathConstants<float>::pi / 2, 5*juce::MathConstants<float>::pi / 2, true);
    reverbWet.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    
    reverbWet.setPopupDisplayEnabled(true, true, this);
    reverbWet.addListener(this);
  
    addAndMakeVisible(reverbWet);
}

MicrobiomeAudioProcessorEditor::~MicrobiomeAudioProcessorEditor()
{
}

//==============================================================================
void MicrobiomeAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll(juce::Colours::lightcoral);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("microbiome", getLocalBounds(), juce::Justification::centred, 1);
}

void MicrobiomeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    reverbWet.setBounds(200-35, 100, 70, 70);
}

void MicrobiomeAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    audioProcessor.setReverbWet(slider->getValue());
}
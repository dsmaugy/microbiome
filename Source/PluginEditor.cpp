/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Constants.h"

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

    addColony.setButtonText("+ Colony");
    removeColony.setButtonText("- Colony");

    addColony.addListener(this);
    removeColony.addListener(this);
  
    addAndMakeVisible(reverbWet);
    addAndMakeVisible(addColony);
    addAndMakeVisible(removeColony);
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

    addColony.setBounds(50, 50, 70, 35);
    removeColony.setBounds(150, 50, 70, 35);
}

void MicrobiomeAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) 
{
    // if (slider == &numColonies) {

    // }
    // audioProcessor.setReverbWet(slider->getValue());
}

void MicrobiomeAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &addColony) {
        DBG("Adding Colony...");
    } else if (button == &removeColony) {
        DBG("Removing Colony...");
    }
}
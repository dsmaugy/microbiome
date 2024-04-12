/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Constants.h"

//==============================================================================
MicrobiomeAudioProcessorEditor::MicrobiomeAudioProcessorEditor(MicrobiomeAudioProcessor& p, juce::AudioProcessorValueTreeState& apvst)
    : AudioProcessorEditor(&p), audioProcessor(p), parameters(apvst), controlWindow(apvst)
{
    
    addColony.setButtonText("->");
    addColony.addListener(this);
    addAndMakeVisible(addColony);

    removeColony.setButtonText("<-");
    removeColony.addListener(this);
    addAndMakeVisible(removeColony);

    //addAndMakeVisible(visualWindow);
    addAndMakeVisible(controlWindow);
    
    setSize(850, 500);
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

    
    g.fillRect(p_topbarRect);
    //g.drawFittedText ("microbiome", getLocalBounds(), juce::Justification::centred, 1);
}

void MicrobiomeAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    visualWindow.setBounds(500, 200, 200, 200);
    addColony.setBounds(50, 50, 70, 35);
    removeColony.setBounds(150, 50, 70, 35);

    p_topbarRect = area.removeFromTop(30);
    controlWindow.setBounds(area.removeFromRight(controlWindow.getWidth()).reduced(10));
}

void MicrobiomeAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) 
{
    // if (slider == &resampleRatio) {
    //     // TODO: set this based off of currently selected colony
    //     // audioProcessor.getEngine().setColonyResampleRatio(0, slider->getValue());
    // }
    // else if (slider == &colonyBufferReadLength) {
    //     audioProcessor.getEngine().setColonyBufferLength(0, slider->getValue());
    // }
    // else if (slider == &colonyBufferReadStart) {
    //     audioProcessor.getEngine().setColonyBufferStart(0, slider->getValue());
    // }
    // // audioProcessor.setReverbWet(slider->getValue());
}

void MicrobiomeAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &addColony) {
        // if (currentColony < MAX_COLONY-1) {
        //     currentColony++;
        //     repaint();
        //     DBG("Switching editor view to colony: " << currentColony);
        // }
        controlWindow.incrementColonyView();
        
    } else if (button == &removeColony) {
        // if (currentColony > 0) {
        //     currentColony--;
        //     repaint();
        //     DBG("Switching editor view to colony: " << currentColony);
        // }
        controlWindow.decrementColonyView();
    }
}

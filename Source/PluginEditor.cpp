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
    : AudioProcessorEditor(&p), audioProcessor(p), parameters(apvst), currentColony(0)
{
    
    for (int i = 0; i < MAX_COLONY; i++) {
        enableColonyButtons[i] = std::make_unique<juce::ToggleButton>("Toggle Colony");
        enableAttachments[i] = std::make_unique<ButtonAttachment>(parameters, PARAMETER_ENABLE_ID(i + 1), *enableColonyButtons[i]);
        addChildComponent(*enableColonyButtons[i]);
    }

    resampleRatio.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    resampleRatio.setRange(0.5, 1.3, 0.01);
    resampleRatio.setValue(1);
    resampleRatio.setRotaryParameters(3 * juce::MathConstants<float>::pi / 2, 5*juce::MathConstants<float>::pi / 2, true);
    resampleRatio.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    resampleRatio.setPopupDisplayEnabled(true, true, this);
    resampleRatio.addListener(this);

    colonyBufferReadLength.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    colonyBufferReadLength.setRange(0.5, COLONY_BUFFER_LENGTH_SEC, 0.01);
    colonyBufferReadLength.setValue(1);
    colonyBufferReadLength.setRotaryParameters(3 * juce::MathConstants<float>::pi / 2, 5 * juce::MathConstants<float>::pi / 2, true);
    colonyBufferReadLength.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    colonyBufferReadLength.setPopupDisplayEnabled(true, true, this);
    colonyBufferReadLength.addListener(this);

    colonyBufferReadStart.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    colonyBufferReadStart.setRange(0.5, COLONY_BUFFER_LENGTH_SEC, 0.01);
    colonyBufferReadStart.setValue(1);
    colonyBufferReadStart.setRotaryParameters(3 * juce::MathConstants<float>::pi / 2, 5 * juce::MathConstants<float>::pi / 2, true);
    colonyBufferReadStart.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    colonyBufferReadStart.setPopupDisplayEnabled(true, true, this);
    colonyBufferReadStart.addListener(this);

    //enableColony.setButtonText("Toggle On");
    //enableAttachment.reset(new ButtonAttachment(parameters, PARAMETER_ENABLE_ID(1), enableColony));

    addColony.setButtonText("->");
    removeColony.setButtonText("<-");

    addColony.addListener(this);
    removeColony.addListener(this);
    
    addAndMakeVisible(resampleRatio);
    addAndMakeVisible(addColony);
    addAndMakeVisible(removeColony);
    addAndMakeVisible(colonyBufferReadLength);
    addAndMakeVisible(colonyBufferReadStart);

    setSize(400, 300);
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

    // TODO: this should go in another component for efficiency
    for (int i = 0; i < MAX_COLONY; i++) {
        if (i == currentColony) {
            enableColonyButtons[i]->setVisible(true);
        } else {
            enableColonyButtons[i]->setVisible(false);
        }
    }
}

void MicrobiomeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    resampleRatio.setBounds(200-35, 100, 70, 70);
    colonyBufferReadLength.setBounds(100, 100, 70, 70);
    colonyBufferReadStart.setBounds(250, 100, 70, 70);

    addColony.setBounds(50, 50, 70, 35);
    removeColony.setBounds(150, 50, 70, 35);

    for (int i = 0; i < MAX_COLONY; i++) {
        enableColonyButtons[i]->setBounds(240, 200, 60, 40);
    }
}

void MicrobiomeAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) 
{
    if (slider == &resampleRatio) {
        // TODO: set this based off of currently selected colony
        audioProcessor.getEngine().setColonyResampleRatio(0, slider->getValue());
    }
    else if (slider == &colonyBufferReadLength) {
        audioProcessor.getEngine().setColonyBufferLength(0, slider->getValue());
    }
    else if (slider == &colonyBufferReadStart) {
        audioProcessor.getEngine().setColonyBufferStart(0, slider->getValue());
    }
    // audioProcessor.setReverbWet(slider->getValue());
}

void MicrobiomeAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &addColony) {
        if (currentColony < MAX_COLONY-1) {
            currentColony++;
            repaint();
            DBG("Switching editor view to colony: " << currentColony);
        }
        
        // audioProcessor.getEngine().addColony();
    } else if (button == &removeColony) {
        if (currentColony > 0) {
            currentColony--;
            repaint();
            DBG("Switching editor view to colony: " << currentColony);
        }
        // audioProcessor.getEngine().removeColony();
    }
}
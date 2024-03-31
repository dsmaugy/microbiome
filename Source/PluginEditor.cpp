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
        colonyComponents[i].push_back(enableColonyButtons[i].get());

        resampleRatioSliders[i] = std::make_unique<juce::Slider>();
        applyRotarySliderStyle(*resampleRatioSliders[i]);
        resampleRatioAttachments[i] = std::make_unique<SliderAttachment>(parameters, PARAMETER_RESAMPLE_RATIO_ID(i+1), *resampleRatioSliders[i]);
        addChildComponent(*resampleRatioSliders[i]);
        colonyComponents[i].push_back(resampleRatioSliders[i].get());

        resampleStartSliders[i] = std::make_unique<juce::Slider>();
        applyRotarySliderStyle(*resampleStartSliders[i]);
        resampleStartAttachments[i] = std::make_unique<SliderAttachment>(parameters, PARAMETER_RESAMPLE_START_ID(i+1), *resampleStartSliders[i]);
        addChildComponent(*resampleStartSliders[i]);
        colonyComponents[i].push_back(resampleStartSliders[i].get());

        colonyPlayControlSliders[i] = std::make_unique<juce::Slider>();
        colonyPlayControlSliders[i]->setSliderStyle(juce::Slider::TwoValueVertical);
        colonyPlayControlAttachments[i] = std::make_unique<juce::TwoValueSliderAttachment>(parameters, 
            PARAMETER_COLONY_START_ID(i+1), 
            PARAMETER_COLONY_END_ID(i+1), 
            *colonyPlayControlSliders[i]);
        addChildComponent(*colonyPlayControlSliders[i]);
        colonyComponents[i].push_back(colonyPlayControlSliders[i].get());

        colonyGainSliders[i] = std::make_unique<juce::Slider>();
        applyRotarySliderStyle(*colonyGainSliders[i]);
        colonyGainAttachments[i] = std::make_unique<SliderAttachment>(parameters, PARAMETER_COLONY_DBFS_ID(i+1), *colonyGainSliders[i]);
        addChildComponent(*colonyGainSliders[i]);
        colonyComponents[i].push_back(colonyGainSliders[i].get());
    }

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
    
    addAndMakeVisible(addColony);
    addAndMakeVisible(removeColony);
    addAndMakeVisible(colonyBufferReadLength);
    addAndMakeVisible(colonyBufferReadStart);

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
    //g.drawFittedText ("microbiome", getLocalBounds(), juce::Justification::centred, 1);
    
    // TODO: this should go in another component for efficiency
    for (int i = 0; i < MAX_COLONY; i++) {
        for (int j = 0; j < colonyComponents[0].size(); j++) { // all colonies should have same # of components
            if (i == currentColony) {
                colonyComponents[i].at(j)->setVisible(true);
            } else {
                colonyComponents[i].at(j)->setVisible(false);
            }
        }
    }
}

void MicrobiomeAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // colonyBufferReadLength.setBounds(100, 100, 70, 70);
    // colonyBufferReadStart.setBounds(250, 100, 70, 70);

    addColony.setBounds(50, 50, 70, 35);
    removeColony.setBounds(150, 50, 70, 35);

    for (int i = 0; i < MAX_COLONY; i++) {
        enableColonyButtons[i]->setBounds(240, 240, 60, 40);
        resampleRatioSliders[i]->setBounds(220, 100, 100, 100);
        resampleStartSliders[i]->setBounds(100, 100, 100, 100);
        colonyPlayControlSliders[i]->setBounds(340, 40, 30, 180);
        colonyGainSliders[i]->setBounds(100, 200, 100, 100);
    }
}

void MicrobiomeAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) 
{
    if (slider == &resampleRatio) {
        // TODO: set this based off of currently selected colony
        // audioProcessor.getEngine().setColonyResampleRatio(0, slider->getValue());
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

void MicrobiomeAudioProcessorEditor::applyRotarySliderStyle(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setRotaryParameters(3 * juce::MathConstants<float>::pi / 2, 5 * juce::MathConstants<float>::pi / 2, true);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 35);
    slider.setPopupDisplayEnabled(true, true, this);
}
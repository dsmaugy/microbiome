/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "../Constants.h"

//==============================================================================
MicrobiomeAudioProcessorEditor::MicrobiomeAudioProcessorEditor(MicrobiomeAudioProcessor& p, juce::AudioProcessorValueTreeState& apvst)
    : AudioProcessorEditor(&p), audioProcessor(p), parameters(apvst), controlWindow(apvst)
{
    
    nextColony.setButtonText("->");
    nextColony.addListener(this);
    addAndMakeVisible(nextColony);

    prevColony.setButtonText("<-");
    prevColony.addListener(this);
    addAndMakeVisible(prevColony);

    addAndMakeVisible(visualWindow);
    addAndMakeVisible(controlWindow);

    for (int i = 0; i < MAX_COLONY; i++) parameters.addParameterListener(PARAMETER_ENABLE_ID(i+1), this);
    
    setLookAndFeel(&laf);
    setSize(850, 500);
}

MicrobiomeAudioProcessorEditor::~MicrobiomeAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void MicrobiomeAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::lightcoral);

    g.setColour (juce::Colours::white);
    g.fillRect(p_topbarRect);

    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds(), 3);

    g.setFont(MicrobiomeResources::m_getTitleFont2(22, juce::Font::bold));
    g.drawFittedText ("microbiome", p_topbarRect.withTrimmedLeft(10), juce::Justification::centredLeft, 1);

    g.setColour(juce::Colours::bisque);
    g.fillRoundedRectangle(p_colMenuRect.toFloat(), 3);

    g.setFont(MicrobiomeResources::m_getTitleFont1(32, 0));
    g.setColour(juce::Colours::black);
    g.drawFittedText(juce::String("Current Colony: ") + juce::String(currentColony+1), p_colMenuRect, juce::Justification::centredTop, 1);
}

void MicrobiomeAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    
    p_topbarRect = area.removeFromTop(30);
    controlWindow.setBounds(area.removeFromRight(500).reduced(10));

    auto colMenu = area.removeFromTop(100).reduced(10);
    p_colMenuRect = juce::Rectangle{colMenu};
    prevColony.setBounds(colMenu.removeFromLeft(colMenu.getWidth()/2).reduced(10).withTrimmedTop(30));
    nextColony.setBounds(colMenu.reduced(10).withTrimmedTop(30));

    visualWindow.setBounds(area.reduced(10, 30));
}

void MicrobiomeAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    // lowkey bad design, the two components share different views of the currentColony variable
    if (button == &nextColony) {
        currentColony = controlWindow.incrementColonyView();
    } else if (button == &prevColony) {
        currentColony = controlWindow.decrementColonyView();
    }

    repaint(p_colMenuRect);
}

void MicrobiomeAudioProcessorEditor::parameterChanged(const juce::String &parameterID, float newValue)
{
    // calls when toggle status changes
    controlWindow.repaint();
}
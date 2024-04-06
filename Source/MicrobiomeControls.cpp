/*
  ==============================================================================

    MicrobiomeControls.cpp
    Created: 6 Apr 2024 11:13:48am
    Author:  darwin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MicrobiomeControls.h"
#include "Constants.h"


//==============================================================================
MicrobiomeControls::MicrobiomeControls(juce::AudioProcessorValueTreeState& apvst) : parameters(apvst)
{
    for (int i = 0; i < MAX_COLONY; i++) {
        colonyModeBox[i] = std::make_unique<juce::ComboBox>("Mode Box");
        colonyModeBox[i]->addItemList(COLONY_MODES, 1);
        colonyModeBox[i]->addSeparator();
        colonyModeBoxAttachment[i] = std::make_unique<juce::ComboBoxParameterAttachment>(*parameters.getParameter(PARAMETER_COLONY_MODE_ID(i + 1)), *colonyModeBox[i]);
        addChildComponent(*colonyModeBox[i]);
        colonyComponents[i].push_back(colonyModeBox[i].get());

        enableColonyButtons[i] = std::make_unique<juce::ToggleButton>("Toggle Colony");
        enableAttachments[i] = std::make_unique<ButtonAttachment>(parameters, PARAMETER_ENABLE_ID(i+1), *enableColonyButtons[i]);
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

        colonyGhostSliders[i] = std::make_unique<juce::Slider>();
        applyRotarySliderStyle(*colonyGhostSliders[i]);
        colonyGhostAttachments[i] = std::make_unique<SliderAttachment>(parameters, PARAMETER_COLONY_GHOST_ID(i+1), *colonyGhostSliders[i]);
        addChildComponent(*colonyGhostSliders[i]);
        colonyComponents[i].push_back(colonyGhostSliders[i].get());

        colonyFilterSliders[i] = std::make_unique<juce::Slider>();
        applyRotarySliderStyle(*colonyFilterSliders[i]);
        colonyFilterAttachments[i] = std::make_unique<SliderAttachment>(parameters, PARAMETER_COLONY_FILTER_ID(i+1), *colonyFilterSliders[i]);
        addChildComponent(*colonyFilterSliders[i]);
        colonyComponents[i].push_back(colonyFilterSliders[i].get());
    }

    engineWetSlider = std::make_unique<juce::Slider>();
    applyRotarySliderStyle(*engineWetSlider);
    engineWetSlider->setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::blanchedalmond);
    engineWetAttachment = std::make_unique<SliderAttachment>(parameters, PARAMETER_ENGINE_WET_ID, *engineWetSlider);
    addAndMakeVisible(*engineWetSlider);

    engineReverbSlider = std::make_unique<juce::Slider>();
    applyRotarySliderStyle(*engineReverbSlider);
    engineReverbSlider->setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::blanchedalmond);
    engineReverbAttachment = std::make_unique<SliderAttachment>(parameters, PARAMETER_ENGINE_REVERB_ID, *engineReverbSlider);
    addAndMakeVisible(*engineReverbSlider);

    engineLushSlider = std::make_unique<juce::Slider>();
    applyRotarySliderStyle(*engineLushSlider);
    engineLushSlider->setColour(juce::Slider::ColourIds::thumbColourId, juce::Colours::blanchedalmond);
    engineLushAttachment = std::make_unique<SliderAttachment>(parameters, PARAMETER_ENGINE_LUSH_ID, *engineLushSlider);
    addAndMakeVisible(*engineLushSlider);

    setSize(500, 500);
}

MicrobiomeControls::~MicrobiomeControls()
{
}

void MicrobiomeControls::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

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

void MicrobiomeControls::resized()
{
    engineWetSlider->setBounds(240, 200, 100, 100);
    engineReverbSlider->setBounds(240, 300, 100, 100);
    engineLushSlider->setBounds(340, 300, 100, 100);

    for (int i = 0; i < MAX_COLONY; i++) {
        enableColonyButtons[i]->setBounds(240, 30, 60, 20);
        resampleRatioSliders[i]->setBounds(220, 100, 100, 100);
        resampleStartSliders[i]->setBounds(100, 100, 100, 100);
        colonyPlayControlSliders[i]->setBounds(340, 40, 30, 180);
        colonyGainSliders[i]->setBounds(100, 200, 100, 100);
        colonyGhostSliders[i]->setBounds(100, 300, 100, 100);
        colonyFilterSliders[i]->setBounds(100, 400, 100, 100);
        colonyModeBox[i]->setBounds(240, 80, 100, 20);
    }

}

void MicrobiomeControls::setViewToColony(int n)
{
    if (n < MAX_COLONY && n >= 0) {
        currentColony = n;
        repaint();
        DBG("Switching editor view to colony: " << currentColony);
    }
}

void MicrobiomeControls::incrementColonyView()
{
    setViewToColony(currentColony+1);
}

void MicrobiomeControls::decrementColonyView()
{
    setViewToColony(currentColony-1);
}

void MicrobiomeControls::applyRotarySliderStyle(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::Rotary);
    // slider.setRotaryParameters(3 * juce::MathConstants<float>::pi / 2, 5 * juce::MathConstants<float>::pi / 2, true);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 35);
    slider.setPopupDisplayEnabled(true, true, this);
}
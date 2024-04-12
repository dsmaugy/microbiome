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
        colonyModeBox[i]->setLookAndFeel(&comboxLAF);
        colonyModeBoxAttachment[i] = std::make_unique<juce::ComboBoxParameterAttachment>(*parameters.getParameter(PARAMETER_COLONY_MODE_ID(i + 1)), *colonyModeBox[i]);
        addChildComponent(*colonyModeBox[i]);
        colonyComponents[i].push_back(colonyModeBox[i].get());

        enableColonyButtons[i] = std::make_unique<juce::ToggleButton>("Toggle Colony");
        enableColonyButtons[i]->setLookAndFeel(&tglButtonLAF);
        enableAttachments[i] = std::make_unique<ButtonAttachment>(parameters, PARAMETER_ENABLE_ID(i+1), *enableColonyButtons[i]);
        addChildComponent(*enableColonyButtons[i]);
        colonyComponents[i].push_back(enableColonyButtons[i].get());

        resampleRatioSliders[i] = std::make_unique<juce::Slider>();
        applyRotarySliderStyle(*resampleRatioSliders[i]);
        resampleRatioAttachments[i] = std::make_unique<SliderAttachment>(parameters, PARAMETER_RESAMPLE_RATIO_ID(i+1), *resampleRatioSliders[i]);
        addChildComponent(*resampleRatioSliders[i]);
        colonyComponents[i].push_back(resampleRatioSliders[i].get());

        resampleStartSliders[i] = std::make_unique<juce::Slider>();
        applyLinearSliderStyle(*resampleStartSliders[i]);
        resampleStartAttachments[i] = std::make_unique<SliderAttachment>(parameters, PARAMETER_RESAMPLE_START_ID(i+1), *resampleStartSliders[i]);
        addChildComponent(*resampleStartSliders[i]);
        colonyComponents[i].push_back(resampleStartSliders[i].get());

        colonyPlayControlSliders[i] = std::make_unique<juce::Slider>();
        colonyPlayControlSliders[i]->setSliderStyle(juce::Slider::TwoValueVertical);
        colonyPlayControlSliders[i]->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        colonyPlayControlSliders[i]->setPopupDisplayEnabled(true, true, this);
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

    engineWetLabel.setText("engine wetness", juce::dontSendNotification);
    engineWetLabel.attachToComponent(engineWetSlider.get(), false);
    engineWetLabel.setPosition(juce::Justification::centredBottom);
    addAndMakeVisible(engineWetLabel);

    engineReverbLabel.setText("engine reverb", juce::dontSendNotification);
    engineReverbLabel.attachToComponent(engineReverbSlider.get(), false);
    engineReverbLabel.setPosition(juce::Justification::centredBottom);
    addAndMakeVisible(engineReverbLabel);

    engineLushLabel.setText("engine lush", juce::dontSendNotification);
    engineLushLabel.attachToComponent(engineLushSlider.get(), false);
    engineLushLabel.setPosition(juce::Justification::centredBottom);
    addAndMakeVisible(engineLushLabel);

    // use one label for all the controls across all colonies
    resampleRatioLabel.setText("sample speed", juce::dontSendNotification);
    resampleRatioLabel.attachToComponent(resampleRatioSliders[currentColony].get(), false);
    resampleRatioLabel.setPosition(juce::Justification::centredBottom);
    addAndMakeVisible(resampleRatioLabel);

    resampleStartLabel.setText("resampler seek", juce::dontSendNotification);
    resampleStartLabel.attachToComponent(resampleStartSliders[currentColony].get(), false);
    resampleStartLabel.setPosition(juce::Justification::centredBottom);
    addAndMakeVisible(resampleStartLabel);

    playControlLabelTop.setText("end", juce::dontSendNotification);
    playControlLabelTop.attachToComponent(colonyPlayControlSliders[currentColony].get(), false);
    playControlLabelTop.setPosition(juce::Justification::centredTop);
    addAndMakeVisible(playControlLabelTop);

    playControlLabelBottom.setText("start", juce::dontSendNotification);
    playControlLabelBottom.attachToComponent(colonyPlayControlSliders[currentColony].get(), false);
    playControlLabelBottom.setPosition(juce::Justification::centredBottom);
    addAndMakeVisible(playControlLabelBottom);

    gainLabel.setText("gain", juce::dontSendNotification);
    gainLabel.attachToComponent(colonyGainSliders[currentColony].get(), false);
    gainLabel.setPosition(juce::Justification::centredBottom);
    addAndMakeVisible(gainLabel);

    ghostLabel.setText("sample ghosts", juce::dontSendNotification);
    ghostLabel.attachToComponent(colonyGhostSliders[currentColony].get(), false);
    ghostLabel.setPosition(juce::Justification::centredBottom);
    addAndMakeVisible(ghostLabel);

    lpfLabel.setText("filter cutoff", juce::dontSendNotification);
    lpfLabel.attachToComponent(colonyFilterSliders[currentColony].get(), false);
    lpfLabel.setPosition(juce::Justification::centredBottom);
    addAndMakeVisible(lpfLabel);

    modeLabel.setText("colony cultivation mode", juce::dontSendNotification);
    modeLabel.attachToComponent(colonyModeBox[currentColony].get(), false);
    modeLabel.setPosition(juce::Justification::centredTop);
    addAndMakeVisible(modeLabel);

    createGUILabels();

    setSize(500, 500);
}

MicrobiomeControls::~MicrobiomeControls()
{
}

void MicrobiomeControls::paint (juce::Graphics& g)
{
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10);

    g.setColour(juce::Colours::darksalmon);
    //g.drawRoundedRectangle(p_engineCtrlsRect.toFloat(), 5, 2);
    g.drawRect(p_engineCtrlsRect.toFloat(), 2);

    
    g.setColour(juce::Colours::lightblue);
    g.fillRoundedRectangle(p_engineCtrlsTtlRect.toFloat(), 5);
    //g.fillRect(p_engineCtrlsTtlRect.toFloat());

    bool currColonyEnabled = *parameters.getRawParameterValue(PARAMETER_ENABLE_ID(currentColony+1)) == 1.0f;

    for (int i = 0; i < MAX_COLONY; i++) {
        for (int j = 0; j < colonyComponents[0].size(); j++) { // all colonies should have same # of components
            if (i == currentColony) {
                colonyComponents[i].at(j)->setVisible(true);
                if (!currColonyEnabled && colonyComponents[i].at(j)->getName() != "Toggle Colony") colonyComponents[i].at(j)->setEnabled(false);
                else colonyComponents[i].at(j)->setEnabled(true);
            } else {
                colonyComponents[i].at(j)->setVisible(false);
            }
        }
    }

    // we only use 1 label for every colony so we update them here
    resampleRatioLabel.attachToComponent(resampleRatioSliders[currentColony].get(), false);
    resampleStartLabel.attachToComponent(resampleStartSliders[currentColony].get(), false);
    playControlLabelTop.attachToComponent(colonyPlayControlSliders[currentColony].get(), false);
    playControlLabelBottom.attachToComponent(colonyPlayControlSliders[currentColony].get(), false);
    gainLabel.attachToComponent(colonyGainSliders[currentColony].get(), false);
    ghostLabel.attachToComponent(colonyGhostSliders[currentColony].get(), false);
    lpfLabel.attachToComponent(colonyFilterSliders[currentColony].get(), false);
    modeLabel.attachToComponent(colonyModeBox[currentColony].get(), false);
}

void MicrobiomeControls::paintOverChildren(juce::Graphics& g)
{
    bool currColonyEnabled = *parameters.getRawParameterValue(PARAMETER_ENABLE_ID(currentColony+1)) == 1.0f;
    g.setColour(juce::Colours::black);
    if (!currColonyEnabled) {
        g.setOpacity(0.90);
    } else {
        g.setOpacity(0.0);
    }
    g.fillRect(p_colonyCtrlsOverlayRect);
}

void MicrobiomeControls::resized()
{
    auto area = getLocalBounds();

    p_engineCtrlsRect = area.removeFromLeft(100);
    int engineCtrlsH = p_engineCtrlsRect.getHeight() / 3;
    
    auto rect = p_engineCtrlsRect;
    engineWetSlider->setBounds(rect.removeFromTop(engineCtrlsH).reduced(0, 20));
    engineReverbSlider->setBounds(rect.removeFromTop(engineCtrlsH).reduced(0, 20));
    engineLushSlider->setBounds(rect.removeFromTop(engineCtrlsH).reduced(0, 20));

    
    auto ctrlTitle = area.removeFromTop(60);
    p_engineCtrlsTtlRect = juce::Rectangle{ ctrlTitle.withTrimmedBottom(20) };
    p_colonyCtrlsOverlayRect = juce::Rectangle{ area.withTrimmedTop(-20)};

    auto enableColonyRect = ctrlTitle.removeFromRight(130).withTrimmedBottom(20);
    auto colonyPlayCtrlRect = area.removeFromRight(85).reduced(10, 20);
    controlTitle.setBounds(ctrlTitle.withTrimmedBottom(20).withTrimmedLeft(10));

    auto topSliderRect = area.removeFromTop(100);
    int topSliderSplit = topSliderRect.getWidth() / 2;
    auto resampleRatioRect = topSliderRect.removeFromLeft(topSliderSplit);
    auto ghostRect = topSliderRect.removeFromLeft(topSliderSplit);

    auto bottomSliderRect = area.removeFromBottom(120).withTrimmedBottom(20);
    int bottomSliderSplit = bottomSliderRect.getWidth() / 2;
    auto filterSlider = bottomSliderRect.removeFromLeft(bottomSliderSplit);
    auto gainSlider = bottomSliderRect.removeFromLeft(bottomSliderSplit);

    auto modeRect = area.removeFromTop(area.getHeight()/2);
    modeRect.removeFromLeft(colonyPlayCtrlRect.getWidth() / 2);
    modeRect = modeRect.withTrimmedTop(70).reduced(35, 5);

    auto seekRect = modeRect.translated(0, area.getCentreY() - area.getY());


    for (int i = 0; i < MAX_COLONY; i++) {
        enableColonyButtons[i]->setBounds(enableColonyRect);
        resampleRatioSliders[i]->setBounds(resampleRatioRect);
        resampleStartSliders[i]->setBounds(seekRect);
        colonyPlayControlSliders[i]->setBounds(colonyPlayCtrlRect);
        colonyGainSliders[i]->setBounds(gainSlider);
        colonyGhostSliders[i]->setBounds(ghostRect);
        colonyFilterSliders[i]->setBounds(filterSlider);
        colonyModeBox[i]->setBounds(modeRect);
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
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setPopupDisplayEnabled(true, true, this);
}

void MicrobiomeControls::applyLinearSliderStyle(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setPopupDisplayEnabled(true, true, this);
}

void MicrobiomeControls::createGUILabels()
{
    controlTitle.setText("Colony Controls", juce::dontSendNotification);
    controlTitle.setFont(MicrobiomeResources::m_getTitleFont1(21, 0));
    addAndMakeVisible(controlTitle);
}
/*
  ==============================================================================

    MicrobiomeWindow.h
    Created: 3 Apr 2024 1:33:10am
    Author:  darwin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#define NUM_NODE_LINES 15

//==============================================================================
/*
*/
class MicrobiomeWindow :    public juce::AnimatedAppComponent

{
public:
    MicrobiomeWindow(juce::AudioProcessorValueTreeState&);
    ~MicrobiomeWindow() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void update() override;	


    // void newOpenGLContextCreated() override;
    // void renderOpenGL() override;
    // void openGLContextClosing() override;

private:
    // juce::OpenGLContext glCtx;
    juce::AudioProcessorValueTreeState& parameters;
    float nodeLineShifts[NUM_NODE_LINES];
    juce::Random rng;

    juce::Path generateTentacle();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MicrobiomeWindow)
};

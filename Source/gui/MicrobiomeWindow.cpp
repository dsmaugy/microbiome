/*
  ==============================================================================

    MicrobiomeWindow.cpp
    Created: 3 Apr 2024 1:33:10am
    Author:  darwin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MicrobiomeWindow.h"

//==============================================================================
MicrobiomeWindow::MicrobiomeWindow()
{
    setOpaque(true);
    glCtx.setRenderer(this);
    // glCtx.setContinuousRepainting(true);
    bool setSwapSuccess = glCtx.setSwapInterval(60);
    glCtx.attachTo(*this);

    DBG("OpenGL: Succesfully set swap interval: " << (int) setSwapSuccess);
}

MicrobiomeWindow::~MicrobiomeWindow()
{
    glCtx.detach();
}

void MicrobiomeWindow::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    //g.setColour (juce::Colours::grey);
    //g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    //g.setColour (juce::Colours::white);
    //g.setFont (14.0f);
    //g.drawText ("MicrobiomeWindow", getLocalBounds(),
    //            juce::Justification::centred, true);   // draw some placeholder text
}

void MicrobiomeWindow::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void MicrobiomeWindow::newOpenGLContextCreated()
{
}

void MicrobiomeWindow::renderOpenGL()
{
}

void MicrobiomeWindow::openGLContextClosing()
{
}
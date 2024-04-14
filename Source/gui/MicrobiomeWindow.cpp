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
    setOpaque(false);
}

MicrobiomeWindow::~MicrobiomeWindow()
{
    // glCtx.detach();
}

void MicrobiomeWindow::paint (juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 8);
}

void MicrobiomeWindow::resized()
{

}

void MicrobiomeWindow::update()
{
    // repaint gets implicitly called in timerCallback
}	

// void MicrobiomeWindow::newOpenGLContextCreated()
// {
// }

// void MicrobiomeWindow::renderOpenGL()
// {
// }

// void MicrobiomeWindow::openGLContextClosing()
// {
// }
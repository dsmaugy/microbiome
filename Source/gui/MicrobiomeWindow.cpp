/*
  ==============================================================================

    MicrobiomeWindow.cpp
    Created: 3 Apr 2024 1:33:10am
    Author:  darwin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MicrobiomeWindow.h"

#define CENTRAL_NODE_SIZE 30

//==============================================================================
MicrobiomeWindow::MicrobiomeWindow()
{
    setOpaque(false);
    setFramesPerSecond(24);
}

MicrobiomeWindow::~MicrobiomeWindow()
{
    // glCtx.detach();
}

void MicrobiomeWindow::paint (juce::Graphics& g)
{
    auto windowRect = getLocalBounds(); 
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(windowRect.toFloat(), 8);

    juce::ColourGradient nodeGradient = juce::ColourGradient{
            juce::Colour{154, 224, 44},
            (float)windowRect.getCentreX(),
            (float)windowRect.getCentreY(),
            juce::Colour{0, 196, 236},
            (float)windowRect.getCentreX() - CENTRAL_NODE_SIZE*2.0f * (float) std::cos(getFrameCounter()),
            (float)windowRect.getCentreY() + CENTRAL_NODE_SIZE*2.0f * (float) std::sin(getFrameCounter()),
            true
    };

    g.setGradientFill(nodeGradient);
    g.fillEllipse(windowRect.getCentreX(), windowRect.getCentreY(), CENTRAL_NODE_SIZE, CENTRAL_NODE_SIZE);

    DBG("Center X: " << windowRect.getCentreX() << " Center Y: " << windowRect.getCentreY());
    DBG("X2: " << nodeGradient.point2.getX() << " Y2: " << nodeGradient.point2.getY());

    // TODO: set this to a different gradient
    g.setColour(nodeGradient.getColourAtPosition(loopCounter * 0.01));
    //g.drawEllipse(windowRect.getCentreX(), windowRect.getCentreY(), CENTRAL_NODE_SIZE, CENTRAL_NODE_SIZE, 3);

    if ((loopCounter += loopDelta) >= 100) loopDelta = -1;
    else if (loopCounter <= 0) loopDelta = 1;

    //DBG("Updating window: " << getFrameCounter());
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
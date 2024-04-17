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

#define PI juce::MathConstants<float>::pi

//==============================================================================
MicrobiomeWindow::MicrobiomeWindow()
{
    setOpaque(false);
    setFramesPerSecond(24);

    for (int i = 0; i < NUM_NODE_LINES; i++) nodeLineShifts[i] = rng.nextFloat() * 2 * PI;
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

    float gradScale = std::sin(getFrameCounter() * 0.05);
    juce::ColourGradient nodeGradient = juce::ColourGradient{
            juce::Colour{154, 224, 44},
            (float)windowRect.getCentreX(),
            (float)windowRect.getCentreY(),
            juce::Colour{0, 196, 236},
            (float)windowRect.getCentreX() + (CENTRAL_NODE_SIZE*0.5f) * gradScale,
            (float)windowRect.getCentreY() + (CENTRAL_NODE_SIZE*0.5f) * gradScale,
            true
    };

    g.setGradientFill(nodeGradient);
    g.fillEllipse(windowRect.withSizeKeepingCentre(CENTRAL_NODE_SIZE, CENTRAL_NODE_SIZE).toFloat());

    // DBG("Center X: " << windowRect.getCentreX() << " Center Y: " << windowRect.getCentreY());
    // DBG("X2: " << nodeGradient.point2.getX() << " Y2: " << nodeGradient.point2.getY());

    // TODO: set this to a different gradient
    g.setColour(nodeGradient.getColourAtPosition((gradScale+1)/2.0f));

    for (int i = 0; i < NUM_NODE_LINES; i++) {
        float shift = (std::sin(getFrameCounter()*0.1 + nodeLineShifts[i]) + 1)/2.0f;
        g.drawLine(
            (float)windowRect.getCentreX() + (CENTRAL_NODE_SIZE*0.5)*std::cos((i/(float)NUM_NODE_LINES)*2*PI), 
            (float)windowRect.getCentreY() + (CENTRAL_NODE_SIZE*0.5)*std::sin((i/(float)NUM_NODE_LINES)*2*PI), 
            (float)windowRect.getCentreX() + (CENTRAL_NODE_SIZE*0.57*shift)*std::cos((i/(float)NUM_NODE_LINES)*2*PI), 
            (float)windowRect.getCentreY() + (CENTRAL_NODE_SIZE*0.57*shift)*std::sin((i/(float)NUM_NODE_LINES)*2*PI), 2.5);
    }

    g.drawEllipse(windowRect.withSizeKeepingCentre(CENTRAL_NODE_SIZE, CENTRAL_NODE_SIZE).toFloat(), 3);

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
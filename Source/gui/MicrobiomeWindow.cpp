/*
  ==============================================================================

    MicrobiomeWindow.cpp
    Created: 3 Apr 2024 1:33:10am
    Author:  darwin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "../Constants.h"
#include "MicrobiomeWindow.h"

#define CENTRAL_NODE_SIZE 40

#define PI juce::MathConstants<float>::pi

//==============================================================================
MicrobiomeWindow::MicrobiomeWindow(juce::AudioProcessorValueTreeState& p) : parameters(p)
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

    for (int i = 0; i < MAX_COLONY; i++) {
        if (*parameters.getRawParameterValue(PARAMETER_ENABLE_ID(i+1)) == 1.0f) {
            // TODO: draw colony appendages here
        }
    }

    g.fillPath(generateTentacle(), juce::AffineTransform().translated(windowRect.getCentreX(), windowRect.getCentreY()));
    //DBG("Updating window: " << getFrameCounter());
}

juce::Path MicrobiomeWindow::generateTentacle()
{
    juce::Path tentacle;

    tentacle.cubicTo(-57.82, -89.35, -54.4, -88.48, -55.5, -88.77);
    tentacle.cubicTo(-59.71, -88.97, -52.12, -80.49, -55.51, -84.27);
    tentacle.cubicTo(-51.35, -82.01, -40.34, -69.81, -45.61, -75.64);
    tentacle.cubicTo(-30.17, -61.81, -24.31, -54.13, -27.56, -58.38);
    tentacle.cubicTo(-17.42, -42.07, -9.68, -19.92, -13.77, -31.42);
    tentacle.cubicTo(-7.52, -13.32, -6.96, 3.27, -7.39, -6.36);
    return tentacle;
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
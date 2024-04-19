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
#define TENTACLE_NUM 30

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
    // juce::AffineTransform().translated(windowRect.getCentreX(), windowRect.getCentreY())
    juce::AffineTransform tentacleTrans = juce::AffineTransform().translated(windowRect.getCentreX() - CENTRAL_NODE_SIZE / 2, windowRect.getCentreY() - CENTRAL_NODE_SIZE / 2);
    g.fillPath(generateTentacle(), tentacleTrans.rotated(PI, windowRect.getCentreX(), windowRect.getCentreY()));
    //DBG("Updating window: " << getFrameCounter());
}

juce::Path MicrobiomeWindow::generateTentacle()
{
    juce::Path tentacle;
    //tentacle.cubicTo(0, -62.054589, -54.88271, -88.216838, -55.979039, -88.472879);
    //tentacle.cubicTo(-2.278549, -0.532142, -4.205171, -0.212992, 0.0076, 4.50335);
    //tentacle.cubicTo(3.376907, 3.780547, 4.120973, 2.275031, 9.898228, 8.650395);
    //tentacle.cubicTo(5.26835, 5.813774, 15.37922, 13.801706, 18.032106, 17.258251);
    //tentacle.cubicTo(3.248295, 4.232327, 10.093375, 16.203423, 13.814965, 26.870763);
    //tentacle.cubicTo(4.03765, 11.57329, 6.0551442, 18.10126, 6.38053, 25.04921);
    //tentacle.cubicTo(0.4466419, 9.5371253, 7.8456, 6.14091, 7.8456, 6.14091);
    //tentacle = juce::Drawable::parseSVGPath("M60,81.8C60,25.8,3.7,0.4,2.5,0.2C0.2-0.3-1.8,0,2.6,4.3c3.5, 3.5, 4.2, 2.1, 10.2, 8c5.4, 5.4, 16, 12.6, 18.5, 15.9c7.5, 9.7, 8.9, 13.4, 14.2, 24.8c4.8, 10.4, 5, 13, 7.5, 20.4C58.8, 91.6, 60, 81.8, 60, 81.8L60, 81.8z");
    //tentacle = juce::Drawable::parseSVGPath("m16,15.9795C16,5.0453.9792.0804.6792.0342.0555-.0617-.4717-.0042.6812.8459c.9242.6814,1.1279.41,2.709,1.559,1.4419,1.0478,4.2563,2.4648,4.9352,3.1104,1.9911,1.8934,2.3684,2.613,3.781,4.8429,1.2916,2.0388,1.3459,2.5365,1.9926,3.983,1.5872,3.5504,1.901,1.6383,1.901,1.6383h0Z");
    tentacle = juce::Drawable::parseSVGPath("m12.6812,16.2152c.6164-.5031,2.0145-5.4573-.3229-9.568C9.0586.8442.0789-.1745.0005.0223c-.0367.0922,1.7826.6986,5.6157,2.8628,2.1695,1.225,3.2689,1.8526,3.9854,2.5945,2.1638,2.2406,2.6346,4.987,2.8079,6.0835.3055,1.9336.1518,4.0137-.1208,4.5627-.0224.0452-.1567.3038-.0906.3579.0771.063.3836-.1872.4831-.2684Z");
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
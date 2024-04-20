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

juce::Path tentaclePaths[] = {
    juce::Drawable::parseSVGPath("m14.7356,12.9145c.2516.049.6516-.655.8824-1.3235,1.2471-3.613-1.162-9.9156-5.2059-11.2941C5.7549-1.2907-.0454,4.0018.0003,4.091c.0244.0477,1.9336-.9803,5.2941-1.9412,2.4911-.7123,3.4316-.7164,4.3235-.3529.2427.0989,1.8033.7348,2.2941,2.1177.191.5381.1122.8854.4412,1.5882.3597.7684.8484,1.1958,1.0588,1.4118,1.7529,1.7985.7344,5.8853,1.3235,6Z"),
    juce::Drawable::parseSVGPath("m4.3442,9.7004C6.8893,7.1554,6.2882.9629,4.0442.1004,3.6365-.0563,2.4326-.3208,0,1.8961c.2661,3.7058.6412,4.508.9412,4.5294.3538.0253.8033-.8894.9412-1.5294.0531-.2463.1317-.6365-.0588-1-.0936-.1786-.1864-.2252-.2872-.3499-.3576-.4423-.3457-1.271-.0658-1.6501.5958-.8069,2.5679-.7424,3.4706.2353.7017.76.5691,1.8544.3529,3.3529-.1455,1.0083-.4814,2.4999-1.3499,4.216,0,0-.2.3-.1.4.1-.1.4-.3.5-.4Z"),
    juce::Drawable::parseSVGPath("m11.4088,15.5c.1554.0063,1.8701-5.4804-.3-9.6C8.7032,1.3333,2.4928.2862,0,0c.1374.2835.3938.7055.8587.9869.6291.3807,1.0854.1684,2.406.3366.5349.0681,1.1922.1562,1.9706.4706.7447.3007,1.2875.6821,1.8235,1.0588.3157.2219.7113.5021,1.1765.9412,0,0,.6521.6155,1.1765,1.3529,1.1471,1.6133,1.5294,4.3025,1.6971,5.7529.3511,3.0378.2176,4.5966.3,4.6Z"),
    juce::Drawable::parseSVGPath("m9.7342,14.5168c.2085.055.4853-.3715.653-.6299.8029-1.2373.5822-2.6594.3971-3.75-.1999-1.1776-.3587-2.1134-1.1029-3.0441-.6227-.7788-1.8834-1.8391-2.8235-1.5-.4271.1541-.4028.4579-.8824.8382-1.0034.7957-3.0076.9708-3.7059-.0441-.5359-.7789-.2409-2.1688.4657-2.5989.44-.2678.714.0346,1.549-.1511.1331-.0296,1.1718-.2757,1.8824-1.1765C6.8977,1.5337,7.0659.1874,6.7549.0192c-.1868-.101-.5448.2239-.668.3357-.6925.6285-.4388,1.2601-1.0085,1.7526-.5587.483-1.3878.3815-1.4879.3679-.5329-.0725-.6358-.3256-1.3062-.4562-.1637-.0319-.7665-.1493-1.1618,0-.9348.3531-1.526,2.5072-.7941,3.8824,1.0833,2.0355,5.1759,2.5723,6.8382,1.5.1986-.1281.6684-.4765,1.0882-.3235.5139.1873.6514,1.0161.7206,1.4706.5053,3.3203.2847,5.8432.7588,5.9682Z"),
    //juce::Drawable::parseSVGPath("m6.2307, 15.5321c.2458.0354.5312 - .3592.653 - .6299.2226 - .4948.0871 - .9948.0355 - 1.1996 - .2964 - 1.1773.1122 - .8213 - .2941 - 3.4706 - .2387 - 1.5562 - .3667 - 2.347 - .7941 - 2.9706 - .306 - .4465 - .6688 - .9759 - 1.3235 - 1.2059 - .4622 - .1624 - .8007 - .0811 - 1.3824 - .3824 - .3375 - .1748 - .6429 - .3317 - .7353 - .6176 - .1647 - .5095.636 - 1.1553, 1.5294 - 1.8529C6.5404, 1.1558, 7.786.4306, 7.6692.1291c - .0898 - .2318 - .9405 - .1002 - 1.5882, 0 - .8865.1371 - 2.0564.443 - 3.4976, 1.2411C1.1069, 2.1878.3283, 2.5954.0869, 3.4907c - .2622.9726.1489, 1.8166.1852, 1.8884.9077, 1.7961, 3.3567.9599, 4.4792, 2.7143.2322.3628.397, 1.6509.7267, 4.2269.2543, 1.987.3528, 3.1543.7527, 3.2119Z"),
    juce::Drawable::parseSVGPath("m12.6812,16.2152c.6164-.5031,2.0145-5.4573-.3229-9.568C9.0586.8442.0789-.1745.0005.0223c-.0367.0922,1.7826.6986,5.6157,2.8628,2.1695,1.225,3.2689,1.8526,3.9854,2.5945,2.1638,2.2406,2.6346,4.987,2.8079,6.0835.3055,1.9336.1518,4.0137-.1208,4.5627-.0224.0452-.1567.3038-.0906.3579.0771.063.3836-.1872.4831-.2684Z")
};

//==============================================================================
MicrobiomeWindow::MicrobiomeWindow(juce::AudioProcessorValueTreeState& p) : parameters(p)
{
    setOpaque(false);
    setFramesPerSecond(24);

    for (int i = 0; i < NUM_NODE_LINES; i++) nodeLineShifts[i] = rng.nextFloat() * 2 * PI;

    for (int i = 0; i < NUM_TENTACLES; i++) tentacles[i] = generateTentacle();
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

    juce::AffineTransform tentacleTrans = juce::AffineTransform().translated(windowRect.getCentreX() - CENTRAL_NODE_SIZE / 1.5, windowRect.getCentreY() - CENTRAL_NODE_SIZE / 1.5);
    g.setColour(juce::Colours::rebeccapurple);
    for (int i = 0; i < NUM_TENTACLES; i++) {
        float shift = (std::sin(getFrameCounter()*0.05 + nodeLineShifts[i%NUM_TENTACLES]) + 1)*0.008;

        g.fillPath(
            tentacles[i].tentacle, 
            tentacleTrans
                .rotated(i * (2 * PI) / NUM_TENTACLES, windowRect.getCentreX(), windowRect.getCentreY())
                .sheared(shift, 0));
    }

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


    for (int i = 0; i < MAX_COLONY; i++) {
        if (*parameters.getRawParameterValue(PARAMETER_ENABLE_ID(i+1)) == 1.0f) {
            // TODO: draw colony appendages here
        }
    }

    g.setColour(nodeGradient.getColourAtPosition((gradScale + 1) / 2.0f));
    g.drawEllipse(windowRect.withSizeKeepingCentre(CENTRAL_NODE_SIZE, CENTRAL_NODE_SIZE).toFloat(), 3);

    //DBG("Updating window: " << getFrameCounter());
}

MicrobiomeWindow::DrawnTentacle MicrobiomeWindow::generateTentacle()
{
    DrawnTentacle dt;
    dt.tentacle = tentaclePaths[rng.nextInt(juce::Range(0, (int)(sizeof(tentaclePaths) / sizeof(tentaclePaths[0]))))];
    dt.transform = juce::AffineTransform();
    
    return dt;
}

void MicrobiomeWindow::resized()
{

}

void MicrobiomeWindow::update()
{
    // repaint gets implicitly called in timerCallback
}	

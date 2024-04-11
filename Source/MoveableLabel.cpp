/*
  ==============================================================================

    MoveableLabel.cpp
    Created: 9 Apr 2024 5:37:57pm
    Author:  darwin

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MoveableLabel.h"
#include "MicrobiomeResources.h"

//==============================================================================
MoveableLabel::MoveableLabel(int j) : justification(j)
{
    setJustificationType(juce::Justification::centred);
    setFont(MicrobiomeResources::m_getLabelFont(26, juce::Font::FontStyleFlags::bold));
}

MoveableLabel::~MoveableLabel()
{
}

void MoveableLabel::setPosition(int j)
{
    justification = j;
}

void MoveableLabel::componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized)
{
    auto& lf = getLookAndFeel();
    auto f = lf.getLabelFont(*this);
    auto borderSize = lf.getLabelBorderSize(*this);

    if (justification == juce::Justification::centredLeft)
    {
        auto width = juce::jmin(juce::roundToInt(f.getStringWidthFloat(getTextValue().toString()) + 0.5f)
            + borderSize.getLeftAndRight(),
            component.getX());

        setBounds(component.getX() - width, component.getY(), width, component.getHeight());
    }
    else if (justification == juce::Justification::centredTop)
    {
        auto height = borderSize.getTopAndBottom() + 6 + juce::roundToInt(f.getHeight() + 0.5f);

        setBounds(component.getX(), component.getY() - height, component.getWidth(), height);
    }
    else if (justification == juce::Justification::centredBottom)
    {
        auto height = borderSize.getTopAndBottom() + 6 + juce::roundToInt(f.getHeight() + 0.5f);
        //DBG("Y: " << component.getY());
        setBounds(component.getX(), component.getY() + component.getHeight() - 10, component.getWidth(), height);
    }
    else 
    {
        DBG("Label justification not supported!");
        jassertfalse;
    }
}
/*
  ==============================================================================

    MoveableLabel.h
    Created: 9 Apr 2024 5:37:57pm
    Author:  darwin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class MoveableLabel  : public juce::Label
{
public:
    MoveableLabel(int j = juce::Justification::centredTop);
    ~MoveableLabel() override;

    void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override;
    void setPosition(int j);

private:
    int justification;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MoveableLabel)
};

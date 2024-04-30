/*
  ==============================================================================

    MicrobiomeResources.h
    Created: 11 Apr 2024 2:27:31pm
    Author:  darwin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

// JUCE autoleak detector triggers here because of static members
class MicrobiomeResources : public juce::DeletedAtShutdown
{
public:
    static juce::Font m_getLabelFont(float fontHeight = 15.0f, int style = juce::Font::FontStyleFlags::plain);

    static juce::Font m_getComboBoxFont(float fontHeight = 10.0f, int style = juce::Font::FontStyleFlags::plain);

    static juce::Font m_getTitleFont1(float fontHeight = 10.0f, int style = juce::Font::FontStyleFlags::plain);

    static juce::Font m_getTitleFont2(float fontHeight = 10.0f, int style = juce::Font::FontStyleFlags::plain);
};
/*
  ==============================================================================

    ModeLAF.h
    Created: 11 Apr 2024 3:38:59pm
    Author:  darwin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MicrobiomeResources.h"

class MicrobiomeLAF : public juce::LookAndFeel_V4
{
public:
    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return getCommonMenuFont();
    }

    juce::Font getPopupMenuFont() override
    {
        return getCommonMenuFont();
    }

private:
    juce::Font getCommonMenuFont()
    {
        return MicrobiomeResources::m_getComboBoxFont(20, juce::Font::FontStyleFlags::bold);
    }
};
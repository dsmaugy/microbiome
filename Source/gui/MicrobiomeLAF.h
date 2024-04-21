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
    MicrobiomeLAF();

    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return getCommonMenuFont();
    }

    juce::Font getPopupMenuFont() override
    {
        return getCommonMenuFont();
    }

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
        const bool isSeparator, const bool isActive,
        const bool isHighlighted, const bool isTicked,
        const bool hasSubMenu, const juce::String& text,
        const juce::String& shortcutKeyText,
        const juce::Drawable* icon, const juce::Colour* const textColourToUse);

    void drawComboBox(juce::Graphics& g, int width, int height, bool,
        int, int, int, int, juce::ComboBox& box);

private:
    juce::Font getCommonMenuFont()
    {
        return MicrobiomeResources::m_getComboBoxFont(20, juce::Font::FontStyleFlags::bold);
    }
};
/*
  ==============================================================================

    MicrobiomeResources.cpp
    Created: 23 Apr 2024 10:48:04am
    Author:  darwin

  ==============================================================================
*/

#include "MicrobiomeResources.h"

static const auto poppinsFont = juce::Typeface::createSystemTypefaceFor(BinaryData::PoppinsRegular_ttf, BinaryData::PoppinsRegular_ttfSize);
static const auto suwannaTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::SuwannaphumRegular_ttf, BinaryData::SuwannaphumRegular_ttfSize);
static const auto robotoTypeface = juce::Typeface::createSystemTypefaceFor(BinaryData::RobotoSlabMedium_ttf, BinaryData::RobotoSlabMedium_ttfSize);

juce::Font MicrobiomeResources::m_getLabelFont(float fontHeight, int style) 
{   
    juce::Font f = juce::Font(poppinsFont);
    f.setHeight(fontHeight);
    f.setStyleFlags(style);
    return f;
}

juce::Font MicrobiomeResources::m_getComboBoxFont(float fontHeight, int style) 
{    
    juce::Font f = juce::Font(suwannaTypeface);
    f.setHeight(fontHeight);
    f.setStyleFlags(style);
    return f;
}

juce::Font MicrobiomeResources::m_getTitleFont1(float fontHeight, int style) 
{    
    juce::Font f = juce::Font(robotoTypeface);
    f.setHeight(fontHeight);
    f.setStyleFlags(style);
    return f;
}

juce::Font MicrobiomeResources::m_getTitleFont2(float fontHeight, int style) 
{
    juce::Font f = juce::Font(robotoTypeface);
    f.setHeight(fontHeight);
    f.setStyleFlags(style);
    return f;
}
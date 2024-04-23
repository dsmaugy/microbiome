/*
  ==============================================================================

    MicrobiomeResources.cpp
    Created: 23 Apr 2024 10:48:04am
    Author:  darwin

  ==============================================================================
*/

#include "MicrobiomeResources.h"


juce::Font MicrobiomeResources::m_getLabelFont(float fontHeight, int style) {
    static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::PoppinsRegular_ttf, BinaryData::PoppinsRegular_ttfSize);
    juce::Font f = juce::Font(typeface);
    f.setHeight(fontHeight);
    f.setStyleFlags(style);
    return f;
}

juce::Font MicrobiomeResources::m_getComboBoxFont(float fontHeight, int style) {
    static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::SuwannaphumRegular_ttf, BinaryData::SuwannaphumRegular_ttfSize);
    juce::Font f = juce::Font(typeface);
    f.setHeight(fontHeight);
    f.setStyleFlags(style);
    return f;
}

juce::Font MicrobiomeResources::m_getTitleFont1(float fontHeight, int style) {
    static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::RobotoSlabMedium_ttf, BinaryData::RobotoSlabMedium_ttfSize);
    juce::Font f = juce::Font(typeface);
    f.setHeight(fontHeight);
    f.setStyleFlags(style);
    return f;
}

juce::Font MicrobiomeResources::m_getTitleFont2(float fontHeight, int style) {
    static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::MuseoModernoRegular_ttf, BinaryData::MuseoModernoRegular_ttfSize);
    juce::Font f = juce::Font(typeface);
    f.setHeight(fontHeight);
    f.setStyleFlags(style);
    return f;
}
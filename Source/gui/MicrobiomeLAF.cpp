/*
  ==============================================================================

    MicrobiomeLAF.cpp
    Created: 20 Apr 2024 10:17:28pm
    Author:  darwin

  ==============================================================================
*/

#include "MicrobiomeLAF.h"
#include "../Constants.h"

using namespace juce;

MicrobiomeLAF::MicrobiomeLAF()
{
    setColour(juce::Slider::ColourIds::rotarySliderFillColourId, Microbiome::baseColorDark);
    setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, Microbiome::baseColorLight);
    setColour(juce::Slider::ColourIds::backgroundColourId, Microbiome::baseColorLight);
    setColour(juce::Slider::ColourIds::trackColourId, Microbiome::baseColorDark);
    setColour(juce::Slider::ColourIds::thumbColourId, Microbiome::interactGreenColor);
    setColour(juce::ComboBox::ColourIds::backgroundColourId, Microbiome::baseColorDark);
    setColour(juce::ComboBox::ColourIds::outlineColourId, Microbiome::baseColorLight);
    setColour(juce::ComboBox::ColourIds::arrowColourId, Microbiome::interactGreenColor);
    setColour(juce::ComboBox::ColourIds::textColourId, Microbiome::textColor);
    setColour(juce::PopupMenu::ColourIds::backgroundColourId, Microbiome::baseColorLight);
    setColour(juce::PopupMenu::ColourIds::highlightedBackgroundColourId, Microbiome::baseColorDark);
    setColour(juce::PopupMenu::ColourIds::textColourId, Microbiome::textColor);
    setColour(juce::TextButton::ColourIds::buttonColourId, Microbiome::baseColorLight);
    setColour(juce::TextButton::ColourIds::buttonOnColourId, Microbiome::baseColorDark);
    setColour(juce::TextButton::ColourIds::textColourOffId, Microbiome::textColor);
    setColour(juce::TextButton::ColourIds::textColourOnId, Microbiome::textColor);
    setColour(juce::Label::ColourIds::textColourId, Microbiome::textColor);
    setColour(juce::ToggleButton::ColourIds::textColourId, Microbiome::textColor);
}

void MicrobiomeLAF::drawPopupMenuItem(Graphics& g, const Rectangle<int>& area,
    const bool isSeparator, const bool isActive,
    const bool isHighlighted, const bool isTicked,
    const bool hasSubMenu, const String& text,
    const String& shortcutKeyText,
    const Drawable* icon, const Colour* const textColourToUse)
{
    if (isSeparator)
    {
        auto r = area.reduced(5, 0);
        r.removeFromTop(roundToInt(((float)r.getHeight() * 0.5f) - 0.5f));

        g.setColour(findColour(PopupMenu::textColourId).withAlpha(0.3f));
        g.fillRect(r.removeFromTop(1));
    }
    else
    {
        auto textColour = (textColourToUse == nullptr ? findColour(PopupMenu::textColourId)
            : *textColourToUse);

        auto r = area.reduced(1);

        if (isHighlighted && isActive)
        {
            g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
            // CUSTOM LAF MODIFICATION, make rounded corners instead of straight
            g.fillRoundedRectangle(r.toFloat(), 10);

            g.setColour(findColour(PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
        }

        r.reduce(jmin(5, area.getWidth() / 20), 0);

        auto font = getPopupMenuFont();

        auto maxFontHeight = (float)r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight(maxFontHeight);

        g.setFont(font);

        auto iconArea = r.removeFromLeft(roundToInt(maxFontHeight)).toFloat();

        if (icon != nullptr)
        {
            icon->drawWithin(g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
            r.removeFromLeft(roundToInt(maxFontHeight * 0.5f));
        }
        else if (isTicked)
        {
            auto tick = getTickShape(1.0f);
            g.fillPath(tick, tick.getTransformToScaleToFit(iconArea.reduced(iconArea.getWidth() / 5, 0).toFloat(), true));
        }

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = static_cast<float> (r.removeFromRight((int)arrowH).getX());
            auto halfH = static_cast<float> (r.getCentreY());

            Path path;
            path.startNewSubPath(x, halfH - arrowH * 0.5f);
            path.lineTo(x + arrowH * 0.6f, halfH);
            path.lineTo(x, halfH + arrowH * 0.5f);

            g.strokePath(path, PathStrokeType(2.0f));
        }

        r.removeFromRight(3);
        g.drawFittedText(text, r, Justification::centredLeft, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight(f2.getHeight() * 0.75f);
            f2.setHorizontalScale(0.95f);
            g.setFont(f2);

            g.drawText(shortcutKeyText, r, Justification::centredRight, true);
        }
    }
}

void MicrobiomeLAF::drawComboBox(Graphics& g, int width, int height, bool,
    int, int, int, int, ComboBox& box)
{
    //auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    // CUSTOM LAF MODIFICATION, hardcode rounded corner instead of cornerSize
    float cornerSize = 10;
    Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(box.findColour(ComboBox::backgroundColourId));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    g.setColour(box.findColour(ComboBox::outlineColourId));
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);

    Rectangle<int> arrowZone(width - 30, 0, 20, height);
    Path path;
    path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
    path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
    path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);

    g.setColour(box.findColour(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath(path, PathStrokeType(2.0f));
}
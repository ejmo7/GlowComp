#include "PluginEditor.h"

GlowCompAudioProcessorEditor::GlowCompAudioProcessorEditor (GlowCompAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    const std::array<juce::String, 6> names { "THRESHOLD", "RATIO", "ATTACK", "RELEASE", "MAKEUP", "MIX" };
    const std::array<juce::String, 6> ids { "threshold", "ratio", "attack", "release", "makeup", "mix" };

    for (size_t i = 0; i < sliders.size(); ++i)
    {
        auto& slider = sliders[i];
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 84, 22);
        slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xfff0a35b));
        slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff384052));
        slider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffffd09a));
        slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xfff2f0ec));
        slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff1a1e28));
        slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff343b4a));
        addAndMakeVisible (slider);

        auto& label = labels[i];
        label.setText (names[i], juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        label.setColour (juce::Label::textColourId, juce::Colour (0xffc7cbd4));
        label.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold)));
        addAndMakeVisible (label);

        attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processor.parameters, ids[i], slider);
    }

    setSize (620, 430);
}

void GlowCompAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff11141b));
    g.setColour (juce::Colour (0xff1b202b));
    g.fillRoundedRectangle (18.0f, 18.0f, static_cast<float> (getWidth() - 36), static_cast<float> (getHeight() - 36), 18.0f);

    g.setColour (juce::Colour (0xfff0a35b));
    g.fillRoundedRectangle (34.0f, 36.0f, 4.0f, 38.0f, 2.0f);
    g.setColour (juce::Colour (0xfff4f1eb));
    g.setFont (juce::Font (juce::FontOptions (26.0f, juce::Font::bold)));
    g.drawText ("GLOWCOMP", 50, 32, 240, 32, juce::Justification::left, false);
    g.setColour (juce::Colour (0xff939baa));
    g.setFont (juce::Font (juce::FontOptions (12.0f)));
    g.drawText ("DYNAMIC CONTROL  /  ANALOG-INSPIRED", 50, 63, 330, 18, juce::Justification::left, false);

    g.setColour (juce::Colour (0xff343b49));
    g.drawLine (34.0f, 94.0f, static_cast<float> (getWidth() - 34), 94.0f, 1.0f);
}

void GlowCompAudioProcessorEditor::resized()
{
    constexpr int columns = 3;
    constexpr int cellWidth = 174;
    constexpr int cellHeight = 142;
    const int startX = (getWidth() - columns * cellWidth) / 2;
    const int startY = 108;

    for (int i = 0; i < static_cast<int> (sliders.size()); ++i)
    {
        const int row = i / columns;
        const int column = i % columns;
        const int x = startX + column * cellWidth;
        const int y = startY + row * cellHeight;
        labels[static_cast<size_t> (i)].setBounds (x, y, cellWidth, 20);
        sliders[static_cast<size_t> (i)].setBounds (x + 25, y + 22, cellWidth - 50, 112);
    }
}

#pragma once

#include "PluginProcessor.h"

class GlowCompAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit GlowCompAudioProcessorEditor (GlowCompAudioProcessor&);
    ~GlowCompAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    GlowCompAudioProcessor& processor;
    std::array<juce::Slider, 6> sliders;
    std::array<juce::Label, 6> labels;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 6> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GlowCompAudioProcessorEditor)
};

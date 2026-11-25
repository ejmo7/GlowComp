#include "PluginProcessor.h"
#include "PluginEditor.h"

GlowCompAudioProcessor::GlowCompAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "GlowComp", createParameterLayout())
{
    threshold = parameters.getRawParameterValue ("threshold");
    ratio = parameters.getRawParameterValue ("ratio");
    attack = parameters.getRawParameterValue ("attack");
    release = parameters.getRawParameterValue ("release");
    makeup = parameters.getRawParameterValue ("makeup");
    mix = parameters.getRawParameterValue ("mix");
}

juce::AudioProcessorValueTreeState::ParameterLayout GlowCompAudioProcessor::createParameterLayout()
{
    using FloatParameter = juce::AudioParameterFloat;
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<FloatParameter> (juce::ParameterID { "threshold", 1 }, "Threshold",
                                                   juce::NormalisableRange<float> (-60.0f, 0.0f, 0.1f), -18.0f, "dB"));
    layout.add (std::make_unique<FloatParameter> (juce::ParameterID { "ratio", 1 }, "Ratio",
                                                   juce::NormalisableRange<float> (1.0f, 20.0f, 0.1f), 4.0f, ":1"));
    layout.add (std::make_unique<FloatParameter> (juce::ParameterID { "attack", 1 }, "Attack",
                                                   juce::NormalisableRange<float> (0.1f, 200.0f, 0.1f, 0.5f), 15.0f, "ms"));
    layout.add (std::make_unique<FloatParameter> (juce::ParameterID { "release", 1 }, "Release",
                                                   juce::NormalisableRange<float> (10.0f, 1000.0f, 1.0f, 0.5f), 180.0f, "ms"));
    layout.add (std::make_unique<FloatParameter> (juce::ParameterID { "makeup", 1 }, "Makeup",
                                                   juce::NormalisableRange<float> (0.0f, 24.0f, 0.1f), 0.0f, "dB"));
    layout.add (std::make_unique<FloatParameter> (juce::ParameterID { "mix", 1 }, "Mix",
                                                   juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 100.0f, "%"));
    return layout;
}

void GlowCompAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (juce::jmax (1, samplesPerBlock));
    spec.numChannels = static_cast<juce::uint32> (getTotalNumOutputChannels());
    compressor.prepare (spec);
    compressor.reset();
    dryBuffer.setSize (getTotalNumOutputChannels(), samplesPerBlock, false, false, true);
}

void GlowCompAudioProcessor::releaseResources()
{
    compressor.reset();
}

bool GlowCompAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    const auto input = layouts.getMainInputChannelSet();
    return input == output && (output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo());
}

void GlowCompAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = juce::jmin (buffer.getNumChannels(), dryBuffer.getNumChannels());
    const int numSamples = buffer.getNumSamples();
    if (numSamples > dryBuffer.getNumSamples())
        dryBuffer.setSize (dryBuffer.getNumChannels(), numSamples, false, false, true);

    for (int channel = 0; channel < numChannels; ++channel)
        dryBuffer.copyFrom (channel, 0, buffer, channel, 0, numSamples);

    compressor.setThreshold (*threshold);
    compressor.setRatio (*ratio);
    compressor.setAttack (*attack);
    compressor.setRelease (*release);

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    compressor.process (context);

    const float makeupGain = juce::Decibels:: decibelsToGain (*makeup);
    const float wet = *mix * 0.01f;
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* output = buffer.getWritePointer (channel);
        const auto* dry = dryBuffer.getReadPointer (channel);
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float processed = output[sample] * makeupGain;
            output[sample] = dry[sample] + wet * (processed - dry[sample]);
        }
    }
}

juce::AudioProcessorEditor* GlowCompAudioProcessor::createEditor()
{
    return new GlowCompAudioProcessorEditor (*this);
}

void GlowCompAudioProcessor::getStateInformation (juce::MemoryBlock& destinationData)
{
    if (auto state = parameters.copyState(); auto xml = state.createXml())
        copyXmlToBinary (*xml, destinationData);
}

void GlowCompAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GlowCompAudioProcessor();
}

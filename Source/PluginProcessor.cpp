/*
  ==============================================================================
    VocalPunch - PluginProcessor.cpp
    (c)(p) 2026 Tully EDM Vibe
  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout VocalPunchProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    vpunch::addParameters (l);
    return l;
}

VocalPunchProcessor::VocalPunchProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
    reader.attach (apvts);
    loadPreset (0);
}

bool VocalPunchProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    const auto in = l.getMainInputChannelSet(), out = l.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::stereo() && out != juce::AudioChannelSet::mono()) return false;
    if (in != juce::AudioChannelSet::stereo() && in != juce::AudioChannelSet::mono()) return false;
    return ! (in == juce::AudioChannelSet::stereo() && out == juce::AudioChannelSet::mono());
}

void VocalPunchProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    chain.prepare (sampleRate, juce::jmax (1, samplesPerBlock));
}

void VocalPunchProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals nd;
    midi.clear();
    const int n = buffer.getNumSamples();
    const int nIn = getTotalNumInputChannels(), nOut = buffer.getNumChannels();
    if (n == 0 || nOut == 0) return;

    double bpm = 120.0;
    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
            if (auto b = pos->getBpm()) bpm = *b;

    float* L = buffer.getWritePointer (0);
    float* R = nOut > 1 ? buffer.getWritePointer (1) : nullptr;
    juce::HeapBlock<float> tmp;
    if (R == nullptr) { tmp.allocate ((size_t) n, true); R = tmp.get(); }
    if (nIn < 2 || R == tmp.get()) std::copy (L, L + n, R);   // mono source: feed both sides

    chain.process (L, R, n, reader.read(), bpm);

    if (nOut == 1) for (int i = 0; i < n; ++i) L[i] = 0.5f * (L[i] + R[i]);
    for (int c = 2; c < nOut; ++c) buffer.clear (c, 0, n);
    scope.push (L, nOut > 1 ? R : L, n);
}

void VocalPunchProcessor::loadPreset (int index)
{
    if (! juce::isPositiveAndBelow (index, (int) vpunch::presets().size())) return;
    // mix, input and output trims are the user's and survive preset changes
    tsyn::applyPreset (apvts, vpunch::presets()[(size_t) index], { "mix", "out_gain", "bypass", "in_gain" });
    currentPreset = index;
}

void VocalPunchProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto st = apvts.copyState();
    st.setProperty ("preset", currentPreset, nullptr);
    if (auto xml = st.createXml()) copyXmlToBinary (*xml, dest);
}

void VocalPunchProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
        {
            auto st = juce::ValueTree::fromXml (*xml);
            currentPreset = juce::jlimit (0, getNumPrograms() - 1, (int) st.getProperty ("preset", 0));
            apvts.replaceState (st);
        }
}

juce::AudioProcessorEditor* VocalPunchProcessor::createEditor() { return new VocalPunchEditor (*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new VocalPunchProcessor(); }

/*
  ==============================================================================
    VocalPunch - PluginProcessor.h
    (c)(p) 2026 Tully EDM Vibe
  ==============================================================================
*/
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "VocalChain.h"
#include "VocalPresets.h"
#include "TullyUI.h"

class VocalPunchProcessor : public juce::AudioProcessor
{
public:
    VocalPunchProcessor();
    ~VocalPunchProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "VocalPunch"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 6.0; }

    int getNumPrograms() override { return (int) vpunch::presets().size(); }
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram (int i) override { loadPreset (i); }
    const juce::String getProgramName (int i) override { return juce::isPositiveAndBelow (i, getNumPrograms()) ? vpunch::presets()[(size_t) i].name : ""; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadPreset (int index);
    int  getCurrentPreset() const noexcept { return currentPreset; }

    juce::AudioProcessorValueTreeState apvts;
    vpunch::Chain chain;
    tui::ScopeBuffer scope;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    vpunch::Reader reader;
    int currentPreset = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalPunchProcessor)
};

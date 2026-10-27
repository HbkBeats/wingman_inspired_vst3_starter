#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class IndependentMusicPluginAudioProcessorEditor final
    : public juce::AudioProcessorEditor,
      private juce::Timer
{
public:
    explicit IndependentMusicPluginAudioProcessorEditor(
        IndependentMusicPluginAudioProcessor&);
    ~IndependentMusicPluginAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateStatus();

    IndependentMusicPluginAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::Label statusLabel;
    juce::Label meterLabel;
    juce::Label captureInfoLabel;

    juce::TextButton startButton { "Start Capture" };
    juce::TextButton stopButton { "Stop Capture" };
    juce::TextButton clearButton { "Clear Capture" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        IndependentMusicPluginAudioProcessorEditor)
};

#include "PluginEditor.h"

IndependentMusicPluginAudioProcessorEditor::
IndependentMusicPluginAudioProcessorEditor(
    IndependentMusicPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    titleLabel.setText("Independent Music Plugin", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(22.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(titleLabel);

    statusLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(statusLabel);

    meterLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(meterLabel);

    captureInfoLabel.setJustificationType(juce::Justification::topLeft);
    captureInfoLabel.setMinimumHorizontalScale(0.8f);
    addAndMakeVisible(captureInfoLabel);

    startButton.onClick = [this] { audioProcessor.startCapture(); };
    stopButton.onClick = [this] { audioProcessor.stopCapture(); };
    clearButton.onClick = [this] { audioProcessor.clearCapture(); };

    addAndMakeVisible(startButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(clearButton);

    setSize(520, 300);
    updateStatus();
    startTimerHz(10);
}

void IndependentMusicPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff17191e));
    g.setColour(juce::Colour(0xff303640));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(8.0f), 10.0f, 1.0f);
}

void IndependentMusicPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    titleLabel.setBounds(area.removeFromTop(38));
    area.removeFromTop(8);
    statusLabel.setBounds(area.removeFromTop(26));
    meterLabel.setBounds(area.removeFromTop(26));
    captureInfoLabel.setBounds(area.removeFromTop(64));

    auto buttons = area.removeFromTop(36);
    startButton.setBounds(buttons.removeFromLeft(150).reduced(2));
    stopButton.setBounds(buttons.removeFromLeft(120).reduced(2));
    clearButton.setBounds(buttons.removeFromLeft(120).reduced(2));
}

void IndependentMusicPluginAudioProcessorEditor::timerCallback()
{
    updateStatus();
}

void IndependentMusicPluginAudioProcessorEditor::updateStatus()
{
    statusLabel.setText(
        audioProcessor.isCapturing() ? "Capture status: RECORDING INPUT"
                                     : "Capture status: stopped",
        juce::dontSendNotification);

    const auto peak = audioProcessor.getInputPeak();
    const auto db = peak > 0.000001f ? 20.0 * std::log10(peak) : -120.0;
    meterLabel.setText(
        "Input peak: " + juce::String(db, 1) + " dBFS",
        juce::dontSendNotification);

    captureInfoLabel.setText(
        "Queued audio: " + juce::String(audioProcessor.getCapturedSeconds(), 2)
        + " seconds\nDropped samples: "
        + juce::String(static_cast<juce::int64>(
            audioProcessor.getDroppedSamples()))
        + "\nMemory buffer only — no audio file is written.",
        juce::dontSendNotification);
}

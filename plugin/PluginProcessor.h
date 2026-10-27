#pragma once

#include <JuceHeader.h>
#include "CaptureBuffer.h"

#include <atomic>
#include <memory>

class IndependentMusicPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    IndependentMusicPluginAudioProcessor();
    ~IndependentMusicPluginAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void startCapture() noexcept;
    void stopCapture() noexcept;
    bool isCapturing() const noexcept;

    float getInputPeak() const noexcept;
    double getCapturedSeconds() const noexcept;
    std::uint64_t getDroppedSamples() const noexcept;
    void clearCapture();

private:
    static constexpr std::size_t captureCapacitySamples = 48000u * 30u;
    CaptureBuffer captureBuffer { captureCapacitySamples };

    std::atomic<bool> captureEnabled { false };
    std::atomic<bool> clearRequested { false };
    std::atomic<float> inputPeak { 0.0f };
    std::atomic<double> sampleRateHz { 48000.0 };
    std::atomic<std::uint64_t> totalCapturedSamples { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        IndependentMusicPluginAudioProcessor)
};

#include "PluginProcessor.h"
#include "PluginEditor.h"

IndependentMusicPluginAudioProcessor::IndependentMusicPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void IndependentMusicPluginAudioProcessor::prepareToPlay(
    double sampleRate, int)
{
    sampleRateHz.store(sampleRate > 0.0 ? sampleRate : 48000.0,
                       std::memory_order_relaxed);
}

void IndependentMusicPluginAudioProcessor::releaseResources()
{
    captureEnabled.store(false, std::memory_order_release);
}

bool IndependentMusicPluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    const auto input = layouts.getMainInputChannelSet();
    const auto output = layouts.getMainOutputChannelSet();

    if (input.isDisabled() || output.isDisabled())
        return false;

    // Support mono or stereo input/output, requiring matching layouts.
    return input == output
        && (input == juce::AudioChannelSet::mono()
            || input == juce::AudioChannelSet::stereo());
}

void IndependentMusicPluginAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    midi.clear();

    // Clearing is requested by the UI but performed by the audio-thread
    // producer, avoiding concurrent writes to the ring buffer's indices.
    // This starter has no consumer thread yet.
    if (clearRequested.exchange(false, std::memory_order_acq_rel))
        captureBuffer.clear();

    float peak = 0.0f;
    const int channels = getTotalNumInputChannels();
    const int samples = buffer.getNumSamples();

    for (int ch = 0; ch < channels; ++ch)
        peak = juce::jmax(peak, buffer.getMagnitude(ch, 0, samples));

    inputPeak.store(peak, std::memory_order_relaxed);

    if (!captureEnabled.load(std::memory_order_acquire))
        return;

    // Downmix to mono without allocating. Push one sample at a time so the
    // callback never needs a temporary buffer or heap allocation.
    const int inputChannels = juce::jmin(getTotalNumInputChannels(),
                                        buffer.getNumChannels());

    if (inputChannels <= 0)
        return;

    std::uint64_t acceptedThisBlock = 0;

    for (int i = 0; i < samples; ++i)
    {
        float mono = 0.0f;
        for (int ch = 0; ch < inputChannels; ++ch)
            mono += buffer.getReadPointer(ch)[i];

        mono /= static_cast<float>(inputChannels);
        acceptedThisBlock += captureBuffer.push(&mono, 1);
    }

    totalCapturedSamples.fetch_add(acceptedThisBlock,
                                   std::memory_order_relaxed);
}

juce::AudioProcessorEditor*
IndependentMusicPluginAudioProcessor::createEditor()
{
    return new IndependentMusicPluginAudioProcessorEditor(*this);
}

void IndependentMusicPluginAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData)
{
    juce::ValueTree state("IndependentMusicPluginState");
    state.setProperty("version", 1, nullptr);
    state.setProperty("captureEnabled", false, nullptr);
    juce::MemoryOutputStream stream(destData, false);
    state.writeToStream(stream);
}

void IndependentMusicPluginAudioProcessor::setStateInformation(
    const void* data, int sizeInBytes)
{
    // Do not resume capture automatically when a project is loaded.
    captureEnabled.store(false, std::memory_order_release);

    if (data == nullptr || sizeInBytes <= 0)
        return;

    auto state = juce::ValueTree::readFromData(data,
                                               static_cast<size_t>(sizeInBytes));
    if (!state.isValid() || state.getType() != "IndependentMusicPluginState")
        return;

    // Future persistent settings can be restored here.
}

void IndependentMusicPluginAudioProcessor::startCapture() noexcept
{
    captureEnabled.store(true, std::memory_order_release);
}

void IndependentMusicPluginAudioProcessor::stopCapture() noexcept
{
    captureEnabled.store(false, std::memory_order_release);
}

bool IndependentMusicPluginAudioProcessor::isCapturing() const noexcept
{
    return captureEnabled.load(std::memory_order_acquire);
}

float IndependentMusicPluginAudioProcessor::getInputPeak() const noexcept
{
    return inputPeak.load(std::memory_order_relaxed);
}

double IndependentMusicPluginAudioProcessor::getCapturedSeconds() const noexcept
{
    const auto rate = sampleRateHz.load(std::memory_order_relaxed);
    if (rate <= 0.0)
        return 0.0;

    return static_cast<double>(captureBuffer.available()) / rate;
}

std::uint64_t IndependentMusicPluginAudioProcessor::getDroppedSamples() const noexcept
{
    return captureBuffer.droppedSamples();
}

void IndependentMusicPluginAudioProcessor::clearCapture()
{
    // Ask the audio-thread producer to clear its ring buffer safely.
    // The clear occurs on the next processBlock callback.
    stopCapture();
    clearRequested.store(true, std::memory_order_release);
    totalCapturedSamples.store(0, std::memory_order_relaxed);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IndependentMusicPluginAudioProcessor();
}

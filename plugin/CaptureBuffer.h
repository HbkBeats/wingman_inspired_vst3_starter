#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <vector>

// A bounded single-producer/single-consumer sample ring buffer.
// Producer: audio callback only.
// Consumer: future analysis worker only.
// Do not call clear() while the producer/consumer are active; stop capture first.
class CaptureBuffer
{
public:
    explicit CaptureBuffer(std::size_t capacitySamples);

    CaptureBuffer(const CaptureBuffer&) = delete;
    CaptureBuffer& operator=(const CaptureBuffer&) = delete;

    // Audio-thread safe when there is exactly one producer.
    // Returns the number of samples accepted; remainder are dropped.
    std::size_t push(const float* source, std::size_t count) noexcept;

    // Worker-thread safe when there is exactly one consumer.
    std::size_t pop(float* destination, std::size_t maxCount) noexcept;

    std::size_t available() const noexcept;
    std::size_t capacity() const noexcept;
    std::uint64_t droppedSamples() const noexcept;
    void clear() noexcept;

private:
    std::vector<float> data;
    const std::size_t bufferCapacity;
    alignas(64) std::atomic<std::uint64_t> writeIndex { 0 };
    alignas(64) std::atomic<std::uint64_t> readIndex { 0 };
    std::atomic<std::uint64_t> dropped { 0 };
};

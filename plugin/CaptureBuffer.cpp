#include "CaptureBuffer.h"

#include <algorithm>
#include <stdexcept>

CaptureBuffer::CaptureBuffer(std::size_t capacitySamples)
    : data(capacitySamples, 0.0f),
      bufferCapacity(capacitySamples)
{
    if (capacitySamples < 2)
        throw std::invalid_argument("CaptureBuffer capacity must be at least 2");
}

std::size_t CaptureBuffer::push(const float* source, std::size_t count) noexcept
{
    if (source == nullptr || count == 0)
        return 0;

    const auto w = writeIndex.load(std::memory_order_relaxed);
    const auto r = readIndex.load(std::memory_order_acquire);
    const auto used = static_cast<std::size_t>(w - r);
    const auto freeSlots = used < bufferCapacity ? bufferCapacity - used : 0;
    const auto accepted = std::min(count, freeSlots);

    for (std::size_t i = 0; i < accepted; ++i)
        data[static_cast<std::size_t>((w + i) % bufferCapacity)] = source[i];

    writeIndex.store(w + accepted, std::memory_order_release);

    if (accepted < count)
        dropped.fetch_add(static_cast<std::uint64_t>(count - accepted),
                          std::memory_order_relaxed);

    return accepted;
}

std::size_t CaptureBuffer::pop(float* destination, std::size_t maxCount) noexcept
{
    if (destination == nullptr || maxCount == 0)
        return 0;

    const auto r = readIndex.load(std::memory_order_relaxed);
    const auto w = writeIndex.load(std::memory_order_acquire);
    const auto queued = static_cast<std::size_t>(w - r);
    const auto count = std::min(maxCount, queued);

    for (std::size_t i = 0; i < count; ++i)
        destination[i] = data[static_cast<std::size_t>((r + i) % bufferCapacity)];

    readIndex.store(r + count, std::memory_order_release);
    return count;
}

std::size_t CaptureBuffer::available() const noexcept
{
    const auto w = writeIndex.load(std::memory_order_acquire);
    const auto r = readIndex.load(std::memory_order_acquire);
    const auto queued = w - r;
    return queued > bufferCapacity ? bufferCapacity : static_cast<std::size_t>(queued);
}

std::size_t CaptureBuffer::capacity() const noexcept
{
    return bufferCapacity;
}

std::uint64_t CaptureBuffer::droppedSamples() const noexcept
{
    return dropped.load(std::memory_order_relaxed);
}

void CaptureBuffer::clear() noexcept
{
    // Call only when producer/consumer are stopped or otherwise quiescent.
    const auto w = writeIndex.load(std::memory_order_relaxed);
    readIndex.store(w, std::memory_order_release);
    dropped.store(0, std::memory_order_relaxed);
}

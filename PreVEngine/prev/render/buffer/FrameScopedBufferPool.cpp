#include "FrameScopedBufferPool.h"

#include "BufferBuilder.h"

namespace prev::render::buffer {
FrameScopedBufferPool::FrameScopedBufferPool(const prev::core::device::Device& device, const prev::core::device::Queue& queue,
    GfxBufferUsageFlags usageFlags, GfxMemoryPropertyFlags memoryProperties,
    uint64_t alignedStride, uint64_t alignment, uint32_t chunkCount)
    : m_device{ device }
    , m_queue{ queue }
    , m_usageFlags{ usageFlags }
    , m_memoryProperties{ memoryProperties }
    , m_alignedStride{ alignedStride }
    , m_alignment{ alignment }
    , m_chunkCount{ chunkCount }
{
}

void FrameScopedBufferPool::BeginFrame(uint32_t frameInFlightIndex)
{
    if (frameInFlightIndex >= m_frames.size()) {
        m_frames.resize(frameInFlightIndex + 1);
    }
    m_currentFrame = frameInFlightIndex;

    // Rewind only when this index is starting a new frame (its prior use is fenced-complete). Calling
    // BeginFrame() again with the same index within a frame — e.g. one shadow renderer driven once per
    // cascade — keeps accumulating, so earlier passes' slices are never overwritten mid-frame.
    if (frameInFlightIndex != m_lastBeganFrame) {
        m_frames[frameInFlightIndex].cursor = 0;
        m_lastBeganFrame = frameInFlightIndex;
    }
}

void FrameScopedBufferPool::GrowRegion(FrameRegion& region)
{
    // Allocate one more backing buffer (a chunk of slices) and append its slices to the region.
    auto backing = BufferBuilder(m_device, m_queue)
                       .SetUsageFlags(m_usageFlags)
                       .SetMemoryProperties(m_memoryProperties)
                       .SetSize(m_alignedStride * m_chunkCount)
                       .SetAlignment(m_alignment)
                       .Build();

    region.slices.reserve(region.slices.size() + m_chunkCount);
    for (uint32_t i = 0; i < m_chunkCount; ++i) {
        region.slices.push_back(backing->Slice(i * m_alignedStride, m_alignedStride));
    }
    region.backings.push_back(std::move(backing));
}

Buffer& FrameScopedBufferPool::Next()
{
    FrameRegion& region = m_frames[m_currentFrame];
    if (region.cursor >= region.slices.size()) {
        GrowRegion(region); // this frame needs more slices than before — allocate another chunk
    }
    Buffer& slice = region.slices[region.cursor];
    ++region.cursor;
    return slice;
}

void FrameScopedBufferPool::TrimRegion(FrameRegion& region)
{
    // Free chunk allocations beyond what this frame used. Those chunks were only ever touched by a
    // prior use of this frame index, which is fenced-complete, and aren't referenced by this frame.
    const uint32_t neededChunks{ (region.cursor + m_chunkCount - 1) / m_chunkCount };
    const size_t keepSlices{ static_cast<size_t>(neededChunks) * m_chunkCount };
    if (region.slices.size() > keepSlices) {
        region.slices.erase(region.slices.begin() + keepSlices, region.slices.end()); // drop slices first (Buffer has no default ctor, so erase not resize)
        region.backings.resize(neededChunks); // then release those chunk allocations
    }
}

void FrameScopedBufferPool::EndFrame()
{
    if (m_currentFrame >= m_frames.size()) {
        return;
    }
    TrimRegion(m_frames[m_currentFrame]);
}
} // namespace prev::render::buffer

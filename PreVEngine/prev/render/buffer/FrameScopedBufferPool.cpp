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

void FrameScopedBufferPool::EndFrame()
{
    // Intentionally does NOT release any backing buffers. Destroying an owning Buffer calls
    // gfxQueueWaitIdle (Buffer::~Buffer); doing that here would stall mid-frame — between the
    // swapchain image acquire and the submit — and on WebGPU/Emscripten gfxQueueWaitIdle yields
    // control back to the browser, which presents and destroys the in-flight swapchain texture.
    // The next submit then references a destroyed texture ("... used in a submit"). So the pool
    // keeps its high-water-mark backings: each frame index's region is rewound and reused in
    // BeginFrame(), never trimmed, and is only freed when the pool itself is destroyed.
}
} // namespace prev::render::buffer

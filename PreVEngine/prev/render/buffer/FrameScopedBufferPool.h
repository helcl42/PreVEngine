#ifndef __FRAME_SCOPED_BUFFER_POOL_H__
#define __FRAME_SCOPED_BUFFER_POOL_H__

#include "Buffer.h"

#include "../../core/device/Device.h"
#include "../../core/device/Queue.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace prev::render::buffer {
class BufferPoolBuilder;

// A buffer pool with one grow-on-demand slice region per frame-in-flight index. It is a per-frame
// linear allocator: BeginFrame() rewinds the region, Next() bumps to the following slice, EndFrame()
// trims the tail. Usage per frame:
//   pool.BeginFrame(frameInFlightIndex);   // rewind this frame's region (its prior use is fenced)
//   auto& slice = pool.Next();             // once per draw; grows the region as needed
//   pool.EndFrame();                       // optional: free chunks the frame didn't use
// Because reuse of a frame index is fenced by the swapchain, a slice is never handed out while the
// GPU might still be reading it — so the pool needs no fixed pre-sizing. The reference returned by
// Next() is valid until the next Next()/BeginFrame()/EndFrame() call.
class FrameScopedBufferPool final {
private:
    FrameScopedBufferPool(const prev::core::device::Device& device, const prev::core::device::Queue& queue,
        GfxBufferUsageFlags usageFlags, GfxMemoryPropertyFlags memoryProperties,
        uint64_t alignedStride, uint64_t alignment, uint32_t chunkCount);

public:
    ~FrameScopedBufferPool() = default;

public:
    void BeginFrame(uint32_t frameInFlightIndex);

    Buffer& Next();

    void EndFrame();

public:
    friend class BufferPoolBuilder;

private:
    // A frame-in-flight index's slices and the chunk allocations backing them.
    struct FrameRegion {
        std::vector<std::unique_ptr<Buffer>> backings; // chunk allocations (one GPU buffer each)
        std::vector<Buffer> slices; // slices carved from the backings
        uint32_t cursor{ 0 }; // slices handed out this frame
    };

    void GrowRegion(FrameRegion& region); // append a chunk of slices (on demand in Next())

    void TrimRegion(FrameRegion& region); // release chunks beyond what the frame used (in EndFrame())

private:
    const prev::core::device::Device& m_device;
    const prev::core::device::Queue& m_queue;
    GfxBufferUsageFlags m_usageFlags;
    GfxMemoryPropertyFlags m_memoryProperties;
    uint64_t m_alignedStride;
    uint64_t m_alignment;
    uint32_t m_chunkCount;

    uint32_t m_currentFrame{ 0 };
    uint32_t m_lastBeganFrame{ UINT32_MAX }; // index whose region was last rewound; detects new-frame boundary
    std::vector<FrameRegion> m_frames;
};
} // namespace prev::render::buffer

#endif // !__FRAME_SCOPED_BUFFER_POOL_H__

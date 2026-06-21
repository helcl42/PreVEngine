#ifndef __FRAME_SCOPED_BIND_GROUP_POOL_H__
#define __FRAME_SCOPED_BIND_GROUP_POOL_H__

#include "IBindGroupPool.h"

#include <cstdint>
#include <vector>

namespace prev::render::shader {
// Bind-group pool with one grow-on-demand region per frame-in-flight index. Usage per frame:
//   pool.BeginFrame(frameInFlightIndex);   // reset this frame's region (its prior use is fenced)
//   auto bg = pool.UpdateNext(descriptor);  // once per draw; grows the region as needed
//   pool.EndFrame();                        // optional: free bind groups the frame didn't reuse
// Because reuse of a frame index is fenced by the swapchain, a slot is never recreated while the
// GPU might still be reading it, so the pool needs no fixed pre-sizing. The owner must ensure the
// GPU is idle (e.g. gfxDeviceWaitIdle) before destroying the pool.
class FrameScopedBindGroupPool final : public IBindGroupPool {
public:
    explicit FrameScopedBindGroupPool(GfxDevice device);

    ~FrameScopedBindGroupPool() override;

public:
    void BeginFrame(uint32_t frameInFlightIndex) override;

    GfxBindGroup UpdateNext(const GfxBindGroupDescriptor& descriptor) override;

    void EndFrame() override;

private:
    GfxDevice m_device{};

    uint32_t m_currentFrame{ 0 };

    uint32_t m_lastBeganFrame{ UINT32_MAX }; // index whose region was last rewound; detects new-frame boundary

    std::vector<std::vector<GfxBindGroup>> m_frameBindGroups; // one growable region per frame index

    std::vector<uint32_t> m_frameCursors; // slots handed out this frame, per frame index
};
} // namespace prev::render::shader

#endif // !__FRAME_SCOPED_BIND_GROUP_POOL_H__

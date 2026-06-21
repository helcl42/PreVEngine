#ifndef __BIND_GROUP_POOL_H__
#define __BIND_GROUP_POOL_H__

#include "IBindGroupPool.h"

#include <vector>

namespace prev::render::shader {
// Fixed-size ring of bind groups. UpdateNext() recreates slots round-robin, so the capacity must
// comfortably exceed the number of distinct bind groups live at once (e.g. draws/frame *
// frames-in-flight, or just 1 for a one-shot/static bind group). Owners that grow with the scene
// per frame should prefer FrameScopedBindGroupPool. The owner must ensure the GPU is idle (e.g.
// gfxDeviceWaitIdle) before destroying the pool.
class BindGroupPool final : public IBindGroupPool {
public:
    BindGroupPool(GfxDevice device, uint32_t capacity);

    ~BindGroupPool() override;

public:
    // A ring has no per-frame state, so the frame signals are intentionally no-ops.
    void BeginFrame(uint32_t /*frameInFlightIndex*/) override { }

    GfxBindGroup UpdateNext(const GfxBindGroupDescriptor& descriptor) override;

    void EndFrame() override { }

private:
    GfxDevice m_device{};

    std::vector<GfxBindGroup> m_bindGroups;

    uint32_t m_currentSlot{ 0 };
};
} // namespace prev::render::shader

#endif // !__BIND_GROUP_POOL_H__

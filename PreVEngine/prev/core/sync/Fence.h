#ifndef __FENCE_H__
#define __FENCE_H__

#include "../device/Device.h"

namespace prev::core::sync {
class Fence final {
public:
    Fence(GfxDevice device, bool signaled = false, const char* label = nullptr);

    ~Fence();

    Fence(const Fence&) = delete;
    Fence& operator=(const Fence&) = delete;
    Fence(Fence&& other) noexcept;
    Fence& operator=(Fence&& other) noexcept;

public:
    void Wait(uint64_t timeoutNs = UINT64_MAX);

    void Reset();

    operator GfxFence() const;

private:
    GfxDevice m_device{};
    GfxFence m_fence{};
};
} // namespace prev::core::sync

#endif

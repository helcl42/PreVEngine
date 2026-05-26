#include "Fence.h"

#include <stdexcept>

namespace prev::core::sync {
Fence::Fence(GfxDevice device, bool signaled, const char* label)
    : m_device{ device }
{
    GfxFenceDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_FENCE_DESCRIPTOR;
    desc.label = label;
    desc.signaled = signaled;
    if (gfxDeviceCreateFence(m_device, &desc, &m_fence) != GFX_RESULT_SUCCESS || !m_fence) {
        throw std::runtime_error("Failed to create fence");
    }
}

Fence::~Fence()
{
    if (m_fence) {
        gfxFenceDestroy(m_fence);
    }
}

Fence::Fence(Fence&& other) noexcept
    : m_device{ other.m_device }
    , m_fence{ other.m_fence }
{
    other.m_fence = nullptr;
}

Fence& Fence::operator=(Fence&& other) noexcept
{
    if (this != &other) {
        if (m_fence) {
            gfxFenceDestroy(m_fence);
        }
        m_device = other.m_device;
        m_fence = other.m_fence;
        other.m_fence = nullptr;
    }
    return *this;
}

void Fence::Wait(uint64_t timeoutNs)
{
    GFXERRCHECK(gfxFenceWait(m_fence, timeoutNs));
}

void Fence::Reset()
{
    GFXERRCHECK(gfxFenceReset(m_fence));
}

Fence::operator GfxFence() const
{
    return m_fence;
}
} // namespace prev::core::sync

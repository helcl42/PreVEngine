#include "BindGroupPool.h"

#include <stdexcept>

namespace prev::render::shader {
BindGroupPool::BindGroupPool(GfxDevice device, uint32_t capacity)
    : m_device{ device }
    , m_bindGroups(capacity, nullptr)
{
    if (capacity == 0) {
        throw std::runtime_error("BindGroupPool requires a capacity >= 1.");
    }
}

BindGroupPool::~BindGroupPool()
{
    for (auto& bg : m_bindGroups) {
        if (bg) {
            gfxBindGroupDestroy(bg);
        }
    }
}

GfxBindGroup BindGroupPool::UpdateNext(const GfxBindGroupDescriptor& descriptor)
{
    GfxBindGroup& slot{ m_bindGroups[m_currentSlot] };
    m_currentSlot = (m_currentSlot + 1) % static_cast<uint32_t>(m_bindGroups.size());

    if (slot) {
        gfxBindGroupDestroy(slot);
        slot = nullptr;
    }
    GFXERRCHECK(gfxDeviceCreateBindGroup(m_device, &descriptor, &slot));
    return slot;
}
} // namespace prev::render::shader

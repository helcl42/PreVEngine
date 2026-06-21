#include "FrameScopedBindGroupPool.h"

#include <stdexcept>

namespace prev::render::shader {
FrameScopedBindGroupPool::FrameScopedBindGroupPool(GfxDevice device)
    : m_device{ device }
{
}

FrameScopedBindGroupPool::~FrameScopedBindGroupPool()
{
    for (auto& region : m_frameBindGroups) {
        for (auto& bg : region) {
            if (bg) {
                gfxBindGroupDestroy(bg);
            }
        }
    }
}

void FrameScopedBindGroupPool::BeginFrame(uint32_t frameInFlightIndex)
{
    if (frameInFlightIndex >= m_frameBindGroups.size()) {
        m_frameBindGroups.resize(frameInFlightIndex + 1);
        m_frameCursors.resize(frameInFlightIndex + 1, 0);
    }
    m_currentFrame = frameInFlightIndex;

    // Rewind only when this index is starting a new frame (its prior use is fenced-complete). Calling
    // BeginFrame() again with the same index within a frame — e.g. one shadow renderer driven once per
    // cascade — keeps accumulating, so earlier passes' bind groups are never overwritten mid-frame.
    if (frameInFlightIndex != m_lastBeganFrame) {
        m_frameCursors[frameInFlightIndex] = 0;
        m_lastBeganFrame = frameInFlightIndex;
    }
}

GfxBindGroup FrameScopedBindGroupPool::UpdateNext(const GfxBindGroupDescriptor& descriptor)
{
    if (m_currentFrame >= m_frameBindGroups.size()) {
        throw std::runtime_error("FrameScopedBindGroupPool::UpdateNext called before BeginFrame.");
    }

    auto& region{ m_frameBindGroups[m_currentFrame] };
    uint32_t& cursor{ m_frameCursors[m_currentFrame] };
    if (cursor >= region.size()) {
        region.push_back(nullptr); // grow on demand for this frame's object count
    }
    GfxBindGroup& slot{ region[cursor] };
    ++cursor;

    if (slot) {
        gfxBindGroupDestroy(slot);
        slot = nullptr;
    }
    GFXERRCHECK(gfxDeviceCreateBindGroup(m_device, &descriptor, &slot));
    return slot;
}

void FrameScopedBindGroupPool::EndFrame()
{
    if (m_currentFrame >= m_frameBindGroups.size()) {
        return;
    }
    // Slots beyond what this frame used are leftovers from a prior use of this frame index, whose
    // submission is already fenced-complete — safe to destroy and shrink the region.
    auto& region{ m_frameBindGroups[m_currentFrame] };
    const uint32_t used{ m_frameCursors[m_currentFrame] };
    for (uint32_t i = used; i < region.size(); ++i) {
        if (region[i]) {
            gfxBindGroupDestroy(region[i]);
        }
    }
    region.resize(used);
}
} // namespace prev::render::shader

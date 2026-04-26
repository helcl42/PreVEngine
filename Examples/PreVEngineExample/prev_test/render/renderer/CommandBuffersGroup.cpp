#include "CommandBuffersGroup.h"

namespace prev_test::render::renderer {
CommandBuffersGroup::CommandBuffersGroup(const prev::core::device::Device& device, const std::vector<std::vector<void*>>& commandPools, const std::vector<std::vector<void*>>& commandBuffers)
    : m_device(device)
    , m_commandPoolGroups(commandPools)
    , m_commandBufferGroups(commandBuffers)
{
}

CommandBuffersGroup::~CommandBuffersGroup()
{
    gfxDeviceWaitIdle(m_device);
    LOGW("CommandBuffersGroup cleanup: parallel rendering command pools not yet supported in gfx API");
}

const std::vector<void*>& CommandBuffersGroup::GetBuffersGroup(const uint32_t index) const
{
    return m_commandBufferGroups[index];
}
} // namespace prev_test::render::renderer
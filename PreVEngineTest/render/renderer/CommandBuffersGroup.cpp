#include "CommandBuffersGroup.h"

namespace prev_test::render::renderer {
CommandBuffersGroup::CommandBuffersGroup(const prev::core::device::Device& device, const std::vector<std::vector<VkCommandPool>>& pools, const std::vector<std::vector<VkCommandBuffer>>& commandBuffers)
    : m_device(device)
    , m_commandPoolGroups(pools)
    , m_commandBufferGroups(commandBuffers)
{
}

CommandBuffersGroup::~CommandBuffersGroup()
{
    vkDeviceWaitIdle(m_device);

    for (auto& poolGroup : m_commandPoolGroups) {
        for (auto& pool : poolGroup) {
            vkDestroyCommandPool(m_device, pool, nullptr);
        }
    }
}

const std::vector<VkCommandBuffer>& CommandBuffersGroup::GetBuffersGroup(const uint32_t index) const
{
    return m_commandBufferGroups[index];
}
} // namespace prev_test::render::renderer
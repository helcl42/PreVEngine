#include "CommandBuffersGroup.h"

namespace prev_test::render::renderer {
CommandBuffersGroup::CommandBuffersGroup(const prev::core::device::Device& device, const VkCommandPool& commandPool, const std::vector<std::vector<VkCommandBuffer>>& commandBuffers)
    : m_device(device)
    , m_commandPool(commandPool)
    , m_commandBufferGroups(commandBuffers)
{
}

CommandBuffersGroup::~CommandBuffersGroup()
{
    vkDeviceWaitIdle(m_device);

    vkDestroyCommandPool(m_device, m_commandPool, nullptr);
}

const std::vector<VkCommandBuffer>& CommandBuffersGroup::GetBuffersGroup(const uint32_t index) const
{
    return m_commandBufferGroups[index];
}
} // namespace prev_test::render::renderer
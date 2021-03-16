#include "CommandBuffersGroup.h"

namespace prev_test::render::renderer {
CommandBuffersGroup::CommandBuffersGroup(const std::vector<std::vector<VkCommandPool> >& pools, const std::vector<std::vector<VkCommandBuffer> >& commandBuffers)
    : m_commandPoolGroups(pools)
    , m_commandBufferGroups(commandBuffers)
{
}

CommandBuffersGroup::~CommandBuffersGroup()
{
    auto device{ prev::core::DeviceProvider::Instance().GetDevice() };

    vkDeviceWaitIdle(*device);

    for (auto& poolGroup : m_commandPoolGroups) {
        for (auto& pool : poolGroup) {
            vkDestroyCommandPool(*device, pool, nullptr);
        }
    }
}

const std::vector<VkCommandBuffer>& CommandBuffersGroup::GetBuffersGroup(const uint32_t index) const
{
    return m_commandBufferGroups.at(index);
}
} // namespace prev_test::render::renderer
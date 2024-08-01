#ifndef __COMMAND_BUFFER_FACTORY_H__
#define __COMMAND_BUFFER_FACTORY_H__

#include <prev/core/device/Device.h>

#include <vector>

namespace prev_test::render::renderer {
class CommandBuffersGroup {
public:
    CommandBuffersGroup(const prev::core::device::Device& device, const std::vector<std::vector<VkCommandPool>>& commandPools, const std::vector<std::vector<VkCommandBuffer>>& commandBuffers);

    ~CommandBuffersGroup();

public:
    const std::vector<VkCommandBuffer>& GetBuffersGroup(const uint32_t index) const;

private:
    const prev::core::device::Device& m_device;

    std::vector<std::vector<VkCommandPool>> m_commandPoolGroups{};

    std::vector<std::vector<VkCommandBuffer>> m_commandBufferGroups{};
};
} // namespace prev_test::render::renderer

#endif // !__COMMAND_BUFFER_FACTORY_H__

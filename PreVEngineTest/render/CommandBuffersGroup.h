#ifndef __COMMAND_BUFFER_FACTORY_H__
#define __COMMAND_BUFFER_FACTORY_H__

#include <prev/core/DeviceProvider.h>

#include <vector>

namespace prev_test::render {
class CommandBuffersGroup {
public:
    CommandBuffersGroup(const std::vector<std::vector<VkCommandPool> >& pools, const std::vector<std::vector<VkCommandBuffer> >& commandBuffers);

    ~CommandBuffersGroup();

public:
    const std::vector<VkCommandBuffer>& GetBuffersGroup(const uint32_t index) const;

private:
    std::vector<std::vector<VkCommandPool> > m_commandPoolGroups{};

    std::vector<std::vector<VkCommandBuffer> > m_commandBufferGroups{};
};
} // namespace prev_test::render

#endif // !__COMMAND_BUFFER_FACTORY_H__

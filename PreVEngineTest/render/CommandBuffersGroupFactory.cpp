#include "CommandBuffersGroupFactory.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render {
std::unique_ptr<CommandBuffersGroup> CommandBuffersGroupFactory::CreateGroup(const prev::core::Queue& queue, const uint32_t dim0Size, const uint32_t groupSize) const
{
    auto device = prev::core::DeviceProvider::Instance().GetDevice();

    std::vector<std::vector<VkCommandPool> > commandPools{};
    std::vector<std::vector<VkCommandBuffer> > commandBuffers{};
    commandPools.reserve(dim0Size);
    commandBuffers.reserve(dim0Size);

    for (uint32_t dim0Index = 0; dim0Index < dim0Size; dim0Index++) {
        std::vector<VkCommandPool> pools{};
        std::vector<VkCommandBuffer> buffers{};
        for (uint32_t shadowRendererIndex = 0; shadowRendererIndex < groupSize; shadowRendererIndex++) {
            auto pool = queue.CreateCommandPool();
            pools.push_back(pool);
            buffers.push_back(prev::util::VkUtils::CreateCommandBuffer(*device, pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY));
        }
        commandPools.push_back(pools);
        commandBuffers.push_back(buffers);
    }

    return std::make_unique<CommandBuffersGroup>(commandPools, commandBuffers);
}
} // namespace prev_test::render
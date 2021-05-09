#include "CommandBuffersGroupFactory.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer {
std::unique_ptr<CommandBuffersGroup> CommandBuffersGroupFactory::CreateGroup(const prev::core::device::Device& device, const prev::core::device::Queue& queue, const uint32_t dim0Size, const uint32_t groupSize, const VkCommandBufferLevel level) const
{
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
            buffers.push_back(prev::util::vk::CreateCommandBuffer(device, pool, level));
        }
        commandPools.push_back(pools);
        commandBuffers.push_back(buffers);
    }

    return std::make_unique<CommandBuffersGroup>(device, commandPools, commandBuffers);
}
} // namespace prev_test::render::renderer
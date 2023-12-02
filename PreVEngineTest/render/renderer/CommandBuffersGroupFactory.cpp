#include "CommandBuffersGroupFactory.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer {
std::unique_ptr<CommandBuffersGroup> CommandBuffersGroupFactory::CreateGroup(const prev::core::device::Device& device, const prev::core::device::Queue& queue, const uint32_t dim0Size, const uint32_t groupSize, const VkCommandBufferLevel level) const
{
    std::vector<std::vector<VkCommandPool>> commandPools(dim0Size);
    std::vector<std::vector<VkCommandBuffer>> commandBuffers(dim0Size);
    for (uint32_t dim0Index = 0; dim0Index < dim0Size; ++dim0Index) {
        std::vector<VkCommandPool> pools(groupSize);
        std::vector<VkCommandBuffer> buffers(groupSize);
        for (uint32_t index = 0; index < groupSize; ++index) {
            VkCommandPool commandPool = prev::util::vk::CreateCommandPool(device, queue.family);
            pools[index] = commandPool;
            buffers[index] = prev::util::vk::CreateCommandBuffer(device, commandPool, level);
        }
        commandPools[dim0Index] = pools;
        commandBuffers[dim0Index] = buffers;
    }
    return std::make_unique<CommandBuffersGroup>(device, commandPools, commandBuffers);
}
} // namespace prev_test::render::renderer
#include "CommandBuffersGroupFactory.h"

#include <prev/util/VkUtils.h>

namespace prev_test::render::renderer {
std::unique_ptr<CommandBuffersGroup> CommandBuffersGroupFactory::CreateGroup(const prev::core::device::Device& device, const prev::core::device::Queue& queue, const uint32_t dim0Size, const uint32_t groupSize, const VkCommandBufferLevel level) const
{
    VkCommandPool commandPool = prev::util::vk::CreateCommandPool(device, queue.family);
    std::vector<std::vector<VkCommandBuffer>> commandBuffers;
    commandBuffers.reserve(dim0Size);
    for (uint32_t dim0Index = 0; dim0Index < dim0Size; ++dim0Index) {
        std::vector<VkCommandBuffer> buffers(groupSize);
        for (uint32_t index = 0; index < groupSize; ++index) {
            buffers[index] = prev::util::vk::CreateCommandBuffer(device, commandPool, level);
        }
        commandBuffers.emplace_back(buffers);
    }

    return std::make_unique<CommandBuffersGroup>(device, commandPool, commandBuffers);
}
} // namespace prev_test::render::renderer
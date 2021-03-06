#ifndef __COMMAND_BUFFER_GROUP_FACTORY_H__
#define __COMMAND_BUFFER_GROUP_FACTORY_H__

#include "CommandBuffersGroup.h"

#include <memory>

namespace prev_test::render::renderer {
class CommandBuffersGroupFactory {
public:
    std::unique_ptr<CommandBuffersGroup> CreateGroup(const prev::core::device::Device& device, const prev::core::device::Queue& queue, const uint32_t dim0Size, const uint32_t groupSize, const VkCommandBufferLevel level) const;
};
} // namespace prev_test::render::renderer

#endif // !__COMMAND_BUFFER_GROUP_FACTORY_H__
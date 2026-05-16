#ifndef __COMMAND_BUFFERS_GROUP_FACTORY_H__
#define __COMMAND_BUFFERS_GROUP_FACTORY_H__

#include "CommandBuffersGroup.h"

#include <prev/render/pass/RenderPass.h>

#include <memory>

namespace prev_test::render::renderer {
class CommandBuffersGroupFactory {
public:
    std::unique_ptr<CommandBuffersGroup> CreateGroup(const prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, uint32_t groupCount, uint32_t groupSize) const;
};
} // namespace prev_test::render::renderer

#endif // !__COMMAND_BUFFERS_GROUP_FACTORY_H__
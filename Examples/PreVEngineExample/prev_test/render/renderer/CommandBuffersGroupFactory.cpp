#include "CommandBuffersGroupFactory.h"

#include <prev/common/Logger.h>

namespace prev_test::render::renderer {
std::unique_ptr<CommandBuffersGroup> CommandBuffersGroupFactory::CreateGroup(const prev::core::device::Device& device, const prev::core::device::Queue& queue, const uint32_t dim0Size, const uint32_t groupSize) const
{
    LOGW("CommandBuffersGroupFactory: parallel rendering not yet supported in gfx API, creating empty group");

    std::vector<std::vector<void*>> commandPools(dim0Size);
    std::vector<std::vector<void*>> commandBuffers(dim0Size);
    for (uint32_t dim0Index = 0; dim0Index < dim0Size; ++dim0Index) {
        commandPools[dim0Index].resize(groupSize, nullptr);
        commandBuffers[dim0Index].resize(groupSize, nullptr);
    }
    return std::make_unique<CommandBuffersGroup>(device, commandPools, commandBuffers);
}
} // namespace prev_test::render::renderer
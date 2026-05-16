#include "CommandBuffersGroupFactory.h"

#include <prev/common/Logger.h>

namespace prev_test::render::renderer {
std::unique_ptr<CommandBuffersGroup> CommandBuffersGroupFactory::CreateGroup(const prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, uint32_t groupCount, uint32_t groupSize) const
{
    std::vector<std::vector<GfxCommandEncoder>> encoderGroups(groupCount);

    GfxRenderBundleEncoderDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_RENDER_BUNDLE_ENCODER_DESCRIPTOR;
    desc.renderPass = renderPass;

    for (uint32_t groupIndex = 0; groupIndex < groupCount; ++groupIndex) {
        encoderGroups[groupIndex].resize(groupSize);
        for (uint32_t i = 0; i < groupSize; ++i) {
            gfxDeviceCreateRenderBundleCommandEncoder(device, &desc, &encoderGroups[groupIndex][i]);
        }
    }

    LOGI("CommandBuffersGroupFactory: created %u x %u render bundle encoders", groupCount, groupSize);
    return std::make_unique<CommandBuffersGroup>(device, std::move(encoderGroups));
}
} // namespace prev_test::render::renderer
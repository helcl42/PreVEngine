#include "CommandBuffersGroup.h"

namespace prev_test::render::renderer {
CommandBuffersGroup::CommandBuffersGroup(const prev::core::device::Device& device, std::vector<std::vector<GfxCommandEncoder>>&& encoders)
    : m_device(device)
    , m_encoderGroups(std::move(encoders))
{
}

CommandBuffersGroup::~CommandBuffersGroup()
{
    gfxDeviceWaitIdle(m_device);
    for (auto& group : m_encoderGroups) {
        for (auto encoder : group) {
            if (encoder) {
                gfxCommandEncoderDestroy(encoder);
            }
        }
    }
}

const std::vector<GfxCommandEncoder>& CommandBuffersGroup::GetEncoders(uint32_t index) const
{
    return m_encoderGroups[index];
}
} // namespace prev_test::render::renderer
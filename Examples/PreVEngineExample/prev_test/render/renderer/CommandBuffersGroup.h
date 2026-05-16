#ifndef __COMMAND_BUFFERS_GROUP_H__
#define __COMMAND_BUFFERS_GROUP_H__

#include <prev/core/device/Device.h>

#include <vector>

namespace prev_test::render::renderer {
class CommandBuffersGroup {
public:
    CommandBuffersGroup(const prev::core::device::Device& device, std::vector<std::vector<GfxCommandEncoder>>&& encoders);

    ~CommandBuffersGroup();

public:
    const std::vector<GfxCommandEncoder>& GetEncoders(uint32_t index) const;

private:
    const prev::core::device::Device& m_device;

    std::vector<std::vector<GfxCommandEncoder>> m_encoderGroups;
};
} // namespace prev_test::render::renderer

#endif // !__COMMAND_BUFFERS_GROUP_H__

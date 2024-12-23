#ifndef __OFF_SCREEN_RENDER_PASS_COMPONENT_FACTORY_H__
#define __OFF_SCREEN_RENDER_PASS_COMPONENT_FACTORY_H__

#include "IOffScreenRenderPassComponent.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

namespace prev_test::component::common {
class OffScreenRenderPassComponentFactory final {
public:
    OffScreenRenderPassComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~OffScreenRenderPassComponentFactory() = default;

public:
    std::unique_ptr<IOffScreenRenderPassComponent> Create(const VkExtent2D& extent, const VkFormat depthFormat, const std::vector<VkFormat>& colorFormats, const uint32_t viewCount) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::common

#endif
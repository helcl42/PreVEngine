#ifndef __OFF_SCREEN_RENDER_PASS_COMPONENT_FACTORY_H__
#define __OFF_SCREEN_RENDER_PASS_COMPONENT_FACTORY_H__

#include "IOffScreenRenderPassComponent.h"

#include <prev/core/device/Device.h>

namespace prev_test::component::common {
class OffScreenRenderPassComponentFactory final {
public:
    OffScreenRenderPassComponentFactory(prev::core::device::Device& device);

    ~OffScreenRenderPassComponentFactory() = default;

public:
    std::unique_ptr<IOffScreenRenderPassComponent> Create(const GfxExtent2D& extent, const GfxFormat depthFormat, const std::vector<GfxFormat>& colorFormats, const uint32_t viewCount) const;

private:
    prev::core::device::Device& m_device;

};
} // namespace prev_test::component::common

#endif
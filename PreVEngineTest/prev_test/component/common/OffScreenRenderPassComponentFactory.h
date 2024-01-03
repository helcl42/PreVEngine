#ifndef __OFF_SCREEN_RENDER_PASS_COMPONENT_FACTORY_H__
#define __OFF_SCREEN_RENDER_PASS_COMPONENT_FACTORY_H__

#include "IOffScreenRenderPassComponent.h"

namespace prev_test::component::common {
class OffScreenRenderPassComponentFactory final {
public:
    std::unique_ptr<IOffScreenRenderPassComponent> Create(const VkExtent2D& extent, const VkFormat depthFormat, const std::vector<VkFormat>& colorFormats) const;
};
} // namespace prev_test::component::common

#endif
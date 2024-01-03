#include "OffScreenRenderPassComponentFactory.h"

#include "OffScreenRenderPassComponent.h"

namespace prev_test::component::common {
std::unique_ptr<IOffScreenRenderPassComponent> OffScreenRenderPassComponentFactory::Create(const VkExtent2D& extent, const VkFormat depthFormat, const std::vector<VkFormat>& colorFormats) const
{
    return std::make_unique<OffScreenRenderPassComponent>(extent, depthFormat, colorFormats);
}
} // namespace prev_test::component::common
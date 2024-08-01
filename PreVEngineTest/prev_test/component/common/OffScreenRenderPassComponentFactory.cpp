#include "OffScreenRenderPassComponentFactory.h"

#include "OffScreenRenderPassComponent.h"

namespace prev_test::component::common {
OffScreenRenderPassComponentFactory::OffScreenRenderPassComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : m_device{ device }
    , m_allocator{ allocator }
{
}

std::unique_ptr<IOffScreenRenderPassComponent> OffScreenRenderPassComponentFactory::Create(const VkExtent2D& extent, const VkFormat depthFormat, const std::vector<VkFormat>& colorFormats) const
{
    return std::make_unique<OffScreenRenderPassComponent>(m_device, m_allocator, extent, depthFormat, colorFormats);
}
} // namespace prev_test::component::common
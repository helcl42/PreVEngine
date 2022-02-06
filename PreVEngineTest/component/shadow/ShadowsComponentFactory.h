#ifndef __SHADOWS_COMPONENT_FACTORY_H__
#define __SHADOWS_COMPONENT_FACTORY_H__

#include "IShadowsComponent.h"
#include "ShadowsCommon.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>

namespace prev_test::component::shadow {
class ShadowsComponentFactory final {
public:
    std::unique_ptr<IShadowsComponent> Create() const;

private:
    std::unique_ptr<prev::render::pass::RenderPass> CreateRenderPass(prev::core::device::Device& device) const;

    std::unique_ptr<prev::core::memory::image::IImageBuffer> CreateDepthBuffer(const VkExtent2D& extent, const uint32_t cascadesCount, prev::core::memory::Allocator& allocator) const;

    std::vector<ShadowsCascade> CreateCascades(const VkExtent2D& extent, const uint32_t cascadesCount, const std::shared_ptr<prev::core::memory::image::IImageBuffer>& depthBuffer, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, prev::core::device::Device& device) const;
};
} // namespace prev_test::component::shadow

#endif
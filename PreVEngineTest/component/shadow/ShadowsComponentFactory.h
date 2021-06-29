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

private:
    static const inline VkFormat DEPTH_FORMAT{ VK_FORMAT_D32_SFLOAT };

    static const inline uint32_t SHADOW_MAP_DIMENSIONS{ 2048 };
};
} // namespace prev_test::component::shadow

#endif
#ifndef __SHADOWS_COMPONENT_FACTORY_H__
#define __SHADOWS_COMPONENT_FACTORY_H__

#include "IShadowsComponent.h"
#include "ShadowsCommon.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/image/IImageBuffer.h>
#include <prev/render/sampler/Sampler.h>

namespace prev_test::component::shadow {
class ShadowsComponentFactory final {
public:
    std::unique_ptr<IShadowsComponent> Create() const;

private:
    std::unique_ptr<prev::render::pass::RenderPass> CreateRenderPass(prev::core::device::Device& device) const;

    std::unique_ptr<prev::render::buffer::image::IImageBuffer> CreateDepthBuffer(const VkExtent2D& extent, const uint32_t cascadesCount, prev::core::memory::Allocator& allocator) const;

    std::unique_ptr<prev::render::sampler::Sampler> CreateSampler(const VkDevice device, const float maxMipMapLevels) const;

    std::vector<ShadowsCascade> CreateCascades(const VkExtent2D& extent, const uint32_t cascadesCount, const std::shared_ptr<prev::render::buffer::image::IImageBuffer>& depthBuffer, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, prev::core::device::Device& device) const;
};
} // namespace prev_test::component::shadow

#endif
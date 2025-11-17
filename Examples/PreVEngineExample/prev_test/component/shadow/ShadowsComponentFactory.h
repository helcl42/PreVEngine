#ifndef __SHADOWS_COMPONENT_FACTORY_H__
#define __SHADOWS_COMPONENT_FACTORY_H__

#include "IShadowsComponent.h"
#include "ShadowsCommon.h"

#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/ImageBuffer.h>

namespace prev_test::component::shadow {
class ShadowsComponentFactory final {
public:
    ShadowsComponentFactory(prev::core::device::Device& device, prev::core::memory::Allocator& allocator);

    ~ShadowsComponentFactory() = default;

public:
    std::unique_ptr<IShadowsComponent> Create() const;

private:
    std::unique_ptr<prev::render::pass::RenderPass> CreateRenderPass() const;

    std::unique_ptr<prev::render::buffer::ImageBuffer> CreateDepthBuffer(const VkExtent2D& extent, const uint32_t cascadesCount) const;

    std::vector<ShadowsCascadeRenderData> CreateCascadesRenderData(const VkExtent2D& extent, const uint32_t cascadesCount, const prev::render::buffer::ImageBuffer& depthBuffer, const prev::render::pass::RenderPass& renderPass) const;

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;
};
} // namespace prev_test::component::shadow

#endif
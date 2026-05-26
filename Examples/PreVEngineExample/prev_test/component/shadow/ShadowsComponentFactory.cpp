#include "ShadowsComponentFactory.h"
#include "ShadowsComponent.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/buffer/ImageBufferViewBuilder.h>
#include <prev/render/framebuffer/FramebufferBuilder.h>
#include <prev/render/pass/RenderPassBuilder.h>

#include <memory>
#include <stdexcept>

namespace prev_test::component::shadow {
ShadowsComponentFactory::ShadowsComponentFactory(prev::core::device::Device& device)
    : m_device{ device }
{
}

std::unique_ptr<IShadowsComponent> ShadowsComponentFactory::Create() const
{
    const GfxExtent2D extent{ SHADOW_MAP_DIMENSIONS, SHADOW_MAP_DIMENSIONS };

    std::shared_ptr<prev::render::pass::RenderPass> renderPass{ CreateRenderPass() };
    std::shared_ptr<prev::render::buffer::ImageBuffer> depthBuffer{ CreateDepthBuffer(extent, CASCADES_COUNT) };
    auto cascadesRenderData{ CreateCascadesRenderData(extent, CASCADES_COUNT, *depthBuffer, *renderPass) };
    auto result{ std::make_unique<ShadowsComponent>(m_device, renderPass, depthBuffer, std::move(cascadesRenderData)) };
    return result;
}

std::unique_ptr<prev::render::pass::RenderPass> ShadowsComponentFactory::CreateRenderPass() const
{
    prev::render::pass::RenderPassBuilder renderPassBuilder{ m_device };
    return renderPassBuilder
        .AddDepthAttachment(DEPTH_FORMAT, GFX_SAMPLE_COUNT_1, MAX_DEPTH, 0, GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_READ_ONLY)
        .AddSubpass({ 0 })
        .Build();
}

std::unique_ptr<prev::render::buffer::ImageBuffer> ShadowsComponentFactory::CreateDepthBuffer(const GfxExtent2D& extent, const uint32_t cascadesCount) const
{
    auto depthBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                           .SetExtent({ extent.width, extent.height, 1 })
                           .SetFormat(DEPTH_FORMAT)
                           .SetType(GFX_TEXTURE_TYPE_2D)
                           .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT | GFX_TEXTURE_USAGE_TEXTURE_BINDING)
                           .SetLayout(GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_ATTACHMENT)
                           .SetViewType(GFX_TEXTURE_VIEW_TYPE_2D_ARRAY)
                           .SetLayerCount(cascadesCount)
                           .Build();
    return depthBuffer;
}

std::vector<ShadowsCascadeRenderData> ShadowsComponentFactory::CreateCascadesRenderData(const GfxExtent2D& extent, const uint32_t cascadesCount, const prev::render::buffer::ImageBuffer& depthBuffer, const prev::render::pass::RenderPass& renderPass) const
{
    std::vector<ShadowsCascadeRenderData> cascades;
    cascades.reserve(cascadesCount);
    for (uint32_t i = 0; i < cascadesCount; ++i) {
        auto view = prev::render::buffer::ImageBufferViewBuilder{ depthBuffer.GetTexture(), GFX_TEXTURE_VIEW_TYPE_2D_ARRAY, depthBuffer.GetFormat(), 1 }
                        .SetMipLevelCount(depthBuffer.GetMipLevels())
                        .SetBaseArrayLayer(i)
                        .SetArrayLayerCount(1)
                        .Build();

        auto framebuffer = prev::render::framebuffer::FramebufferBuilder{ m_device, renderPass }
                               .SetExtent({ extent.width, extent.height })
                               .SetDepthStencilAttachment(*view)
                               .Build();

        ShadowsCascadeRenderData renderData;
        renderData.framebuffer = std::move(framebuffer);
        renderData.textureView = std::move(view);
        cascades.emplace_back(std::move(renderData));
    }
    return cascades;
}
} // namespace prev_test::component::shadow
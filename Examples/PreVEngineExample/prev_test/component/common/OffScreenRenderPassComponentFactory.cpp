#include "OffScreenRenderPassComponentFactory.h"

#include "OffScreenRenderPassComponent.h"

#include <prev/render/buffer/ImageBufferBuilder.h>
#include <prev/render/framebuffer/FramebufferBuilder.h>
#include <prev/render/pass/RenderPassBuilder.h>

#include <stdexcept>

namespace prev_test::component::common {
OffScreenRenderPassComponentFactory::OffScreenRenderPassComponentFactory(prev::core::device::Device& device)
    : m_device{ device }
{
}

std::unique_ptr<IOffScreenRenderPassComponent> OffScreenRenderPassComponentFactory::Create(const GfxExtent2D& extent, const GfxFormat depthFormat, const std::vector<GfxFormat>& colorFormats, const uint32_t viewCount) const
{
    const GfxTextureViewType imageViewType{ viewCount > 1 ? GFX_TEXTURE_VIEW_TYPE_2D_ARRAY : GFX_TEXTURE_VIEW_TYPE_2D };

    // Create render pass
    prev::render::pass::RenderPassBuilder renderPassBuilder{ m_device };
    renderPassBuilder.SetViewCount(viewCount);

    std::vector<uint32_t> attachmentIndices;
    uint32_t attachmentIndex{ 0 };

    if (depthFormat != GFX_FORMAT_UNDEFINED) {
        renderPassBuilder.AddDepthAttachment(depthFormat, GFX_SAMPLE_COUNT_1, MAX_DEPTH, GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_READ_ONLY);
        attachmentIndices.push_back(attachmentIndex++);
    }
    for (const auto colorFormat : colorFormats) {
        renderPassBuilder.AddColorAttachment(colorFormat, GFX_SAMPLE_COUNT_1, { 0.5f, 0.5f, 0.5f, 1.0f }, GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY);
        attachmentIndices.push_back(attachmentIndex++);
    }

    std::shared_ptr<prev::render::pass::RenderPass> renderPass = renderPassBuilder
                                                                     .AddSubpass(attachmentIndices)
                                                                     .Build();

    // Create image buffers
    std::shared_ptr<prev::render::buffer::ImageBuffer> depthBuffer{};
    if (depthFormat != GFX_FORMAT_UNDEFINED) {
        depthBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                          .SetExtent({ extent.width, extent.height, 1 })
                          .SetFormat(depthFormat)
                          .SetType(GFX_TEXTURE_TYPE_2D)
                          .SetViewType(imageViewType)
                          .SetLayerCount(viewCount)
                          .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT | GFX_TEXTURE_USAGE_TEXTURE_BINDING)
                          .SetLayout(GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_READ_ONLY)
                          .Build();
    }

    std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>> colorBuffers{};
    for (const auto colorFormat : colorFormats) {
        auto colorImageBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
                                    .SetExtent({ extent.width, extent.height, 1 })
                                    .SetFormat(colorFormat)
                                    .SetType(GFX_TEXTURE_TYPE_2D)
                                    .SetViewType(imageViewType)
                                    .SetLayerCount(viewCount)
                                    .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT | GFX_TEXTURE_USAGE_TEXTURE_BINDING)
                                    .SetLayout(GFX_TEXTURE_LAYOUT_SHADER_READ_ONLY)
                                    .Build();
        colorBuffers.emplace_back(std::move(colorImageBuffer));
    }

    // Create framebuffer
    prev::render::framebuffer::FramebufferBuilder framebufferBuilder{ m_device, *renderPass };
    framebufferBuilder.SetExtent({ extent.width, extent.height });

    if (depthBuffer) {
        framebufferBuilder.SetDepthStencilAttachment(depthBuffer->GetTextureView());
    }
    for (const auto& colorBuffer : colorBuffers) {
        framebufferBuilder.AddColorAttachment(colorBuffer->GetTextureView());
    }

    auto frameBuffer = framebufferBuilder.Build();

    return std::make_unique<OffScreenRenderPassComponent>(m_device, extent, renderPass, depthBuffer, colorBuffers, std::move(frameBuffer));
}
} // namespace prev_test::component::common

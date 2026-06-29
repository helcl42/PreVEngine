#include "SwapchainTargets.h"

#include "../buffer/ImageBufferBuilder.h"
#include "../framebuffer/FramebufferBuilder.h"

#include <cassert>

namespace prev::render::swapchain {

std::unique_ptr<buffer::ImageBuffer> CreateRenderAttachment(core::device::Device& device, GfxExtent2D extent, GfxFormat format, GfxSampleCount sampleCount, uint32_t viewCount)
{
    const GfxTextureViewType viewType{ (viewCount > 1) ? GFX_TEXTURE_VIEW_TYPE_2D_ARRAY : GFX_TEXTURE_VIEW_TYPE_2D };
    return buffer::ImageBufferBuilder{ device, device.GetQueue(core::device::QueueType::GRAPHICS) }
        .SetExtent({ extent.width, extent.height, 1 })
        .SetFormat(format)
        .SetType(GFX_TEXTURE_TYPE_2D)
        .SetViewType(viewType)
        .SetSampleCount(sampleCount)
        .SetLayerCount(viewCount)
        .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT)
        .Build();
}

SwapchainTargets::SwapchainTargets(core::device::Device& device, pass::RenderPass& renderPass, GfxExtent2D extent, GfxFormat colorFormat, GfxFormat depthFormat, GfxSampleCount sampleCount, uint32_t viewCount, bool createSharedDepth)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_extent{ extent }
    , m_sampleCount{ sampleCount }
{
    if (createSharedDepth) {
        m_depth = CreateRenderAttachment(device, extent, depthFormat, sampleCount, viewCount);
    }
    if (sampleCount > GFX_SAMPLE_COUNT_1) {
        m_msaaColor = CreateRenderAttachment(device, extent, colorFormat, sampleCount, viewCount);
        m_msaaDepth = CreateRenderAttachment(device, extent, depthFormat, sampleCount, viewCount);
    }
}

std::unique_ptr<framebuffer::Framebuffer> SwapchainTargets::CreateFramebuffer(GfxTextureView colorView, GfxTextureView depthView) const
{
    GfxTextureView fbColor{};
    GfxTextureView fbColorResolve{};
    GfxTextureView fbDepth{};
    if (m_sampleCount > GFX_SAMPLE_COUNT_1) {
        fbColor = m_msaaColor->GetTextureView();
        fbColorResolve = colorView; // resolve the multisampled color into the per-image image
        fbDepth = m_msaaDepth->GetTextureView();
    } else {
        // Non-MSAA: use the caller's depth view, or the shared depth. One must exist - a swapchain built
        // without a shared depth (e.g. XR using runtime depth) must pass its own depth view here.
        assert((depthView || m_depth) && "non-MSAA framebuffer needs a depth view or a shared depth");
        fbColor = colorView;
        fbDepth = depthView ? depthView : m_depth->GetTextureView();
    }

    return framebuffer::FramebufferBuilder{ m_device, m_renderPass }
        .SetExtent(m_extent)
        .AddColorAttachment(fbColor, fbColorResolve)
        .SetDepthStencilAttachment(fbDepth)
        .Build();
}

} // namespace prev::render::swapchain

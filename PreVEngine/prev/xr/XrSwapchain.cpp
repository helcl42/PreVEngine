#include "XrSwapchain.h"

#ifdef ENABLE_XR

#include "../render/buffer/ImageBufferBuilder.h"
#include "../render/buffer/ImageBufferViewBuilder.h"
#include "../render/framebuffer/FramebufferBuilder.h"

#include "../common/Logger.h"

namespace prev::xr {
XrSwapchain::XrSwapchain(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, xr::IXr& xr, GfxSampleCount sampleCount)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_xr{ xr }
    , m_sampleCount{ sampleCount }
    , m_graphicsQueue{ m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
{
    m_extent = m_xr.GetExtent();
    CreateResources();
    LOGI("XR Swapchain created: %ux%u", m_extent.width, m_extent.height);
}

XrSwapchain::~XrSwapchain()
{
    m_device.WaitIdle();
    DestroyResources();
    LOGI("XR Swapchain destroyed");
}

void XrSwapchain::CreateResources()
{
    const GfxFormat colorFormat = m_xr.GetColorFormat();
    const GfxFormat depthFormat = m_xr.GetDepthFormat();
    const uint32_t viewCount = m_xr.GetViewCount();
    const GfxTextureViewType viewType = (viewCount > 1) ? GFX_TEXTURE_VIEW_TYPE_2D_ARRAY : GFX_TEXTURE_VIEW_TYPE_2D;

    const auto& graphicsQueue = m_device.GetQueue(prev::core::device::QueueType::GRAPHICS);

    // MSAA resources (shared across all frames)
    if (m_sampleCount > GFX_SAMPLE_COUNT_1) {
        m_msaaColorBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, graphicsQueue }
                                .SetExtent({ m_extent.width, m_extent.height, 1 })
                                .SetFormat(colorFormat)
                                .SetType(GFX_TEXTURE_TYPE_2D)
                                .SetViewType(viewType)
                                .SetSampleCount(m_sampleCount)
                                .SetLayerCount(viewCount)
                                .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT)
                                .Build();

        m_msaaDepthBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, graphicsQueue }
                                .SetExtent({ m_extent.width, m_extent.height, 1 })
                                .SetFormat(depthFormat)
                                .SetType(GFX_TEXTURE_TYPE_2D)
                                .SetViewType(viewType)
                                .SetSampleCount(m_sampleCount)
                                .SetLayerCount(viewCount)
                                .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT)
                                .Build();
    }

    // Get pre-imported OpenXR color textures
    const auto colorTextures = m_xr.GetColorImages();
    const auto swapchainImagesCount = static_cast<uint32_t>(colorTextures.size());

    // Get depth textures (from OpenXR or create our own)
    const bool hasXrDepth = m_xr.HasDepthImages();
    std::vector<GfxTexture> depthTextures;

    if (hasXrDepth) {
        depthTextures = m_xr.GetDepthImages();
    } else {
        // Create a shared depth texture
        const GfxSampleCount depthSampleCount = (m_sampleCount > GFX_SAMPLE_COUNT_1) ? m_sampleCount : GFX_SAMPLE_COUNT_1;
        m_ownedDepthBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, graphicsQueue }
                                 .SetExtent({ m_extent.width, m_extent.height, 1 })
                                 .SetFormat(depthFormat)
                                 .SetType(GFX_TEXTURE_TYPE_2D)
                                 .SetViewType(viewType)
                                 .SetSampleCount(depthSampleCount)
                                 .SetLayerCount(viewCount)
                                 .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT)
                                 .Build();
    }

    m_swapchainBuffers.resize(swapchainImagesCount);
    for (uint32_t i = 0; i < swapchainImagesCount; ++i) {
        auto& sb = m_swapchainBuffers[i];

        // Color texture from OpenXR (already imported by IXr)
        {
            sb.colorTexture = colorTextures[i];

            sb.colorView = prev::render::buffer::ImageBufferViewBuilder{ sb.colorTexture, viewType, colorFormat, viewCount }
                               .Build();
        }

        // Depth: use pre-imported from OpenXR or use shared owned depth
        if (hasXrDepth) {
            sb.depthTexture = depthTextures[i];

            sb.depthView = prev::render::buffer::ImageBufferViewBuilder{ sb.depthTexture, viewType, depthFormat, viewCount }
                               .Build();
        } else {
            // Create a view from the shared owned depth buffer
            sb.depthTexture = m_ownedDepthBuffer->GetTexture();
            sb.depthView = prev::render::buffer::ImageBufferViewBuilder{ *m_ownedDepthBuffer }
                               .Build();
        }

        // Create framebuffer
        GfxTextureView colorView{};
        GfxTextureView depthView{};
        GfxTextureView colorResolve{};
        if (m_sampleCount > GFX_SAMPLE_COUNT_1) {
            colorView = m_msaaColorBuffer->GetTextureView();
            colorResolve = *sb.colorView;
            depthView = m_msaaDepthBuffer->GetTextureView();
        } else {
            colorView = *sb.colorView;
            depthView = *sb.depthView;
        }

        sb.framebuffer = prev::render::framebuffer::FramebufferBuilder{ m_device, m_renderPass }
                             .SetExtent(m_extent)
                             .AddColorAttachment(colorView, colorResolve)
                             .SetDepthStencilAttachment(depthView)
                             .Build();

        // Command encoder
        GfxCommandEncoderDescriptor ceDesc{};
        ceDesc.sType = GFX_STRUCTURE_TYPE_COMMAND_ENCODER_DESCRIPTOR;
        ceDesc.label = "XrCommandEncoder";
        GFXERRCHECK(gfxDeviceCreateCommandEncoder(m_device, &ceDesc, &sb.commandEncoder));

        // Fence (signaled initially)
        sb.fence = std::make_unique<prev::core::sync::Fence>(m_device, true, "XrFence");
    }
}

void XrSwapchain::DestroyResources()
{
    for (auto& sb : m_swapchainBuffers) {
        sb.fence.reset();
        if (sb.commandEncoder) {
            gfxCommandEncoderDestroy(sb.commandEncoder);
        }
        sb.framebuffer.reset();
        sb.colorView.reset();
        sb.depthView.reset();
        // colorTexture/depthTexture owned by IXr or m_ownedDepthBuffer — don't destroy here
    }
    m_swapchainBuffers.clear();

    m_ownedDepthBuffer.reset();
    m_msaaDepthBuffer.reset();
    m_msaaColorBuffer.reset();
}

bool XrSwapchain::BeginFrame(prev::render::swapchain::FrameContext& outContext)
{
    m_acquiredIndex = m_xr.GetCurrentSwapchainIndex();

    auto& sb = m_swapchainBuffers[m_acquiredIndex];

    sb.fence->Wait();
    sb.fence->Reset();
    GFXERRCHECK(gfxCommandEncoderBegin(sb.commandEncoder));

    outContext.frameBuffer = *sb.framebuffer;
    outContext.commandEncoder = sb.commandEncoder;
    outContext.index = m_acquiredIndex;
    return true;
}

void XrSwapchain::EndFrame()
{
    auto& sb = m_swapchainBuffers[m_acquiredIndex];

    GFXERRCHECK(gfxCommandEncoderEnd(sb.commandEncoder));

    GfxCommandEncoder encoders[] = { sb.commandEncoder };

    GfxSubmitDescriptor submitDesc{};
    submitDesc.sType = GFX_STRUCTURE_TYPE_SUBMIT_DESCRIPTOR;
    submitDesc.commandEncoders = encoders;
    submitDesc.commandEncoderCount = 1;
    submitDesc.signalFence = *sb.fence;
    GFXERRCHECK(m_graphicsQueue.Submit(&submitDesc));
}

GfxExtent2D XrSwapchain::GetExtent() const
{
    return m_extent;
}

uint32_t XrSwapchain::GetImageCount() const
{
    return static_cast<uint32_t>(m_swapchainBuffers.size());
}

void XrSwapchain::Print() const
{
    LOGI("XR Swapchain:");
    LOGI("\tExtent  = %u x %u", m_extent.width, m_extent.height);
    LOGI("\tBuffers = %zu", m_swapchainBuffers.size());
    LOGI("\tViews   = %u", m_xr.GetViewCount());
}
} // namespace prev::xr

#endif
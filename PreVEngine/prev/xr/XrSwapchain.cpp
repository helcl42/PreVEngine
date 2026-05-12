#include "XrSwapchain.h"

#ifdef ENABLE_XR

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

    // MSAA resources (shared across all frames)
    if (m_sampleCount > GFX_SAMPLE_COUNT_1) {
        // MSAA color
        {
            GfxTextureDescriptor texDesc{};
            texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
            texDesc.label = "XrMsaaColor";
            texDesc.type = GFX_TEXTURE_TYPE_2D;
            texDesc.size = { m_extent.width, m_extent.height, 1 };
            texDesc.arrayLayerCount = viewCount;
            texDesc.mipLevelCount = 1;
            texDesc.sampleCount = m_sampleCount;
            texDesc.format = colorFormat;
            texDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
            GFXERRCHECK(gfxDeviceCreateTexture(m_device, &texDesc, &m_msaaColorTexture));

            GfxTextureViewDescriptor viewDesc{};
            viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
            viewDesc.label = "XrMsaaColorView";
            viewDesc.viewType = viewType;
            viewDesc.format = colorFormat;
            viewDesc.baseMipLevel = 0;
            viewDesc.mipLevelCount = 1;
            viewDesc.baseArrayLayer = 0;
            viewDesc.arrayLayerCount = viewCount;
            GFXERRCHECK(gfxTextureCreateView(m_msaaColorTexture, &viewDesc, &m_msaaColorView));
        }

        // MSAA depth
        {
            GfxTextureDescriptor texDesc{};
            texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
            texDesc.label = "XrMsaaDepth";
            texDesc.type = GFX_TEXTURE_TYPE_2D;
            texDesc.size = { m_extent.width, m_extent.height, 1 };
            texDesc.arrayLayerCount = viewCount;
            texDesc.mipLevelCount = 1;
            texDesc.sampleCount = m_sampleCount;
            texDesc.format = depthFormat;
            texDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
            GFXERRCHECK(gfxDeviceCreateTexture(m_device, &texDesc, &m_msaaDepthTexture));

            GfxTextureViewDescriptor viewDesc{};
            viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
            viewDesc.label = "XrMsaaDepthView";
            viewDesc.viewType = viewType;
            viewDesc.format = depthFormat;
            viewDesc.baseMipLevel = 0;
            viewDesc.mipLevelCount = 1;
            viewDesc.baseArrayLayer = 0;
            viewDesc.arrayLayerCount = viewCount;
            GFXERRCHECK(gfxTextureCreateView(m_msaaDepthTexture, &viewDesc, &m_msaaDepthView));
        }
    }

    // Get pre-imported OpenXR color textures
    const auto colorTextures = m_xr.GetColorImages();
    const auto swapchainImagesCount = static_cast<uint32_t>(colorTextures.size());

    // Get depth textures (from OpenXR or create our own)
    const bool hasXrDepth = m_xr.HasDepthImages();
    std::vector<GfxTexture> depthTextures;
    GfxTexture ownedDepthTexture{};
    GfxTextureView ownedDepthView{};

    if (hasXrDepth) {
        depthTextures = m_xr.GetDepthImages();
    } else {
        // Create a shared depth texture
        GfxTextureDescriptor texDesc{};
        texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
        texDesc.label = "XrDepth";
        texDesc.type = GFX_TEXTURE_TYPE_2D;
        texDesc.size = { m_extent.width, m_extent.height, 1 };
        texDesc.arrayLayerCount = viewCount;
        texDesc.mipLevelCount = 1;
        texDesc.sampleCount = (m_sampleCount > GFX_SAMPLE_COUNT_1) ? m_sampleCount : GFX_SAMPLE_COUNT_1;
        texDesc.format = depthFormat;
        texDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
        GFXERRCHECK(gfxDeviceCreateTexture(m_device, &texDesc, &ownedDepthTexture));

        GfxTextureViewDescriptor viewDesc{};
        viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
        viewDesc.label = "XrDepthView";
        viewDesc.viewType = viewType;
        viewDesc.format = depthFormat;
        viewDesc.baseMipLevel = 0;
        viewDesc.mipLevelCount = 1;
        viewDesc.baseArrayLayer = 0;
        viewDesc.arrayLayerCount = viewCount;
        GFXERRCHECK(gfxTextureCreateView(ownedDepthTexture, &viewDesc, &ownedDepthView));
    }

    m_swapchainBuffers.resize(swapchainImagesCount);
    for (uint32_t i = 0; i < swapchainImagesCount; ++i) {
        auto& sb = m_swapchainBuffers[i];

        // Color texture from OpenXR (already imported by IXr)
        {
            sb.colorTexture = colorTextures[i];

            GfxTextureViewDescriptor viewDesc{};
            viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
            viewDesc.label = "XrColorView";
            viewDesc.viewType = viewType;
            viewDesc.format = colorFormat;
            viewDesc.baseMipLevel = 0;
            viewDesc.mipLevelCount = 1;
            viewDesc.baseArrayLayer = 0;
            viewDesc.arrayLayerCount = viewCount;
            GFXERRCHECK(gfxTextureCreateView(sb.colorTexture, &viewDesc, &sb.colorView));
        }

        // Depth: use pre-imported from OpenXR or use shared owned depth
        if (hasXrDepth) {
            sb.depthTexture = depthTextures[i];

            GfxTextureViewDescriptor viewDesc{};
            viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
            viewDesc.label = "XrDepthView";
            viewDesc.viewType = viewType;
            viewDesc.format = depthFormat;
            viewDesc.baseMipLevel = 0;
            viewDesc.mipLevelCount = 1;
            viewDesc.baseArrayLayer = 0;
            viewDesc.arrayLayerCount = viewCount;
            GFXERRCHECK(gfxTextureCreateView(sb.depthTexture, &viewDesc, &sb.depthView));
        } else {
            // Use shared depth (store in first buffer, others reference same)
            sb.depthTexture = ownedDepthTexture;
            sb.depthView = ownedDepthView;
        }

        // Create framebuffer
        GfxFramebufferAttachment colorAttachment{};
        GfxFramebufferAttachment depthAttachment{};
        if (m_sampleCount > GFX_SAMPLE_COUNT_1) {
            colorAttachment.view = m_msaaColorView;
            colorAttachment.resolveTarget = sb.colorView;
            depthAttachment.view = m_msaaDepthView;
            depthAttachment.resolveTarget = nullptr;
        } else {
            colorAttachment.view = sb.colorView;
            colorAttachment.resolveTarget = nullptr;
            depthAttachment.view = sb.depthView;
            depthAttachment.resolveTarget = nullptr;
        }

        GfxFramebufferDescriptor fbDesc{};
        fbDesc.sType = GFX_STRUCTURE_TYPE_FRAMEBUFFER_DESCRIPTOR;
        fbDesc.label = "XrFramebuffer";
        fbDesc.renderPass = m_renderPass;
        fbDesc.colorAttachments = &colorAttachment;
        fbDesc.colorAttachmentCount = 1;
        fbDesc.depthStencilAttachment = depthAttachment;
        fbDesc.extent = m_extent;
        GFXERRCHECK(gfxDeviceCreateFramebuffer(m_device, &fbDesc, &sb.framebuffer));

        // Command encoder
        GfxCommandEncoderDescriptor ceDesc{};
        ceDesc.sType = GFX_STRUCTURE_TYPE_COMMAND_ENCODER_DESCRIPTOR;
        ceDesc.label = "XrCommandEncoder";
        GFXERRCHECK(gfxDeviceCreateCommandEncoder(m_device, &ceDesc, &sb.commandEncoder));

        // Fence (signaled initially)
        GfxFenceDescriptor fenceDesc{};
        fenceDesc.sType = GFX_STRUCTURE_TYPE_FENCE_DESCRIPTOR;
        fenceDesc.label = "XrFence";
        fenceDesc.signaled = true;
        GFXERRCHECK(gfxDeviceCreateFence(m_device, &fenceDesc, &sb.fence));
    }
}

void XrSwapchain::DestroyResources()
{
    // Track whether depth is shared (non-XR-provided)
    const bool hasXrDepth = m_xr.HasDepthImages();
    GfxTexture sharedDepthTexture{};
    GfxTextureView sharedDepthView{};

    if (!hasXrDepth && !m_swapchainBuffers.empty()) {
        sharedDepthTexture = m_swapchainBuffers[0].depthTexture;
        sharedDepthView = m_swapchainBuffers[0].depthView;
    }

    for (auto& sb : m_swapchainBuffers) {
        if (sb.fence) {
            gfxFenceDestroy(sb.fence);
        }
        if (sb.commandEncoder) {
            gfxCommandEncoderDestroy(sb.commandEncoder);
        }
        if (sb.framebuffer) {
            gfxFramebufferDestroy(sb.framebuffer);
        }
        if (sb.colorView) {
            gfxTextureViewDestroy(sb.colorView);
        }
        // colorTexture owned by IXr - don't destroy
        if (hasXrDepth) {
            if (sb.depthView) {
                gfxTextureViewDestroy(sb.depthView);
            }
            // depthTexture owned by IXr - don't destroy
        }
    }
    m_swapchainBuffers.clear();

    // Destroy shared depth (only once)
    if (!hasXrDepth) {
        if (sharedDepthView) {
            gfxTextureViewDestroy(sharedDepthView);
        }
        if (sharedDepthTexture) {
            gfxTextureDestroy(sharedDepthTexture);
        }
    }

    if (m_msaaDepthView) {
        gfxTextureViewDestroy(m_msaaDepthView);
        m_msaaDepthView = nullptr;
    }
    if (m_msaaDepthTexture) {
        gfxTextureDestroy(m_msaaDepthTexture);
        m_msaaDepthTexture = nullptr;
    }
    if (m_msaaColorView) {
        gfxTextureViewDestroy(m_msaaColorView);
        m_msaaColorView = nullptr;
    }
    if (m_msaaColorTexture) {
        gfxTextureDestroy(m_msaaColorTexture);
        m_msaaColorTexture = nullptr;
    }
}

bool XrSwapchain::BeginFrame(prev::render::swapchain::FrameContext& outContext)
{
    m_acquiredIndex = m_xr.GetCurrentSwapchainIndex();

    auto& sb = m_swapchainBuffers[m_acquiredIndex];

    GFXERRCHECK(gfxFenceWait(sb.fence, UINT64_MAX));
    GFXERRCHECK(gfxFenceReset(sb.fence));
    GFXERRCHECK(gfxCommandEncoderBegin(sb.commandEncoder));

    outContext.frameBuffer = sb.framebuffer;
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
    submitDesc.signalFence = sb.fence;
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
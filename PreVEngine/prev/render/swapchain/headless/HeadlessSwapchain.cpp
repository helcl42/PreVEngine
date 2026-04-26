#include "HeadlessSwapchain.h"

#include "../../../common/Logger.h"

namespace prev::render::swapchain::headless {
HeadlessSwapchain::HeadlessSwapchain(core::device::Device& device, pass::RenderPass& renderPass, GfxExtent2D extent, uint32_t imageCount, uint32_t viewCount)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_graphicsQueue{ device.GetQueue(core::device::QueueType::GRAPHICS) }
    , m_extent{ extent }
    , m_viewCount{ viewCount }
{
    m_frameIndex = util::CircularIndex<uint32_t>{ imageCount };
    CreateResources();
    LOGI("HeadlessSwapchain created: %ux%u, %u images", m_extent.width, m_extent.height, imageCount);
}

HeadlessSwapchain::~HeadlessSwapchain()
{
    m_device.WaitIdle();
    DestroyResources();
    LOGI("HeadlessSwapchain destroyed");
}

void HeadlessSwapchain::CreateResources()
{
    const GfxFormat colorFormat = m_renderPass.GetColorFormat();
    const GfxFormat depthFormat = m_renderPass.GetDepthFormat();
    const GfxSampleCount sampleCount = m_renderPass.GetSampleCount();
    const GfxTextureViewType viewType = (m_viewCount > 1) ? GFX_TEXTURE_VIEW_TYPE_2D_ARRAY : GFX_TEXTURE_VIEW_TYPE_2D;

    // Shared depth texture
    {
        GfxTextureDescriptor texDesc{};
        texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
        texDesc.label = "HeadlessDepth";
        texDesc.type = GFX_TEXTURE_TYPE_2D;
        texDesc.size = { m_extent.width, m_extent.height, 1 };
        texDesc.arrayLayerCount = m_viewCount;
        texDesc.mipLevelCount = 1;
        texDesc.sampleCount = sampleCount;
        texDesc.format = depthFormat;
        texDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
        GFXERRCHECK(gfxDeviceCreateTexture(m_device, &texDesc, &m_depthTexture));

        GfxTextureViewDescriptor viewDesc{};
        viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
        viewDesc.label = "HeadlessDepthView";
        viewDesc.viewType = viewType;
        viewDesc.format = depthFormat;
        viewDesc.baseMipLevel = 0;
        viewDesc.mipLevelCount = 1;
        viewDesc.baseArrayLayer = 0;
        viewDesc.arrayLayerCount = m_viewCount;
        GFXERRCHECK(gfxTextureCreateView(m_depthTexture, &viewDesc, &m_depthView));
    }

    if (sampleCount > GFX_SAMPLE_COUNT_1) {
        // MSAA color
        {
            GfxTextureDescriptor texDesc{};
            texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
            texDesc.label = "HeadlessMsaaColor";
            texDesc.type = GFX_TEXTURE_TYPE_2D;
            texDesc.size = { m_extent.width, m_extent.height, 1 };
            texDesc.arrayLayerCount = m_viewCount;
            texDesc.mipLevelCount = 1;
            texDesc.sampleCount = sampleCount;
            texDesc.format = colorFormat;
            texDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
            GFXERRCHECK(gfxDeviceCreateTexture(m_device, &texDesc, &m_msaaColorTexture));

            GfxTextureViewDescriptor viewDesc{};
            viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
            viewDesc.label = "HeadlessMsaaColorView";
            viewDesc.viewType = viewType;
            viewDesc.format = colorFormat;
            viewDesc.baseMipLevel = 0;
            viewDesc.mipLevelCount = 1;
            viewDesc.baseArrayLayer = 0;
            viewDesc.arrayLayerCount = m_viewCount;
            GFXERRCHECK(gfxTextureCreateView(m_msaaColorTexture, &viewDesc, &m_msaaColorView));
        }

        // MSAA depth
        {
            GfxTextureDescriptor texDesc{};
            texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
            texDesc.label = "HeadlessMsaaDepth";
            texDesc.type = GFX_TEXTURE_TYPE_2D;
            texDesc.size = { m_extent.width, m_extent.height, 1 };
            texDesc.arrayLayerCount = m_viewCount;
            texDesc.mipLevelCount = 1;
            texDesc.sampleCount = sampleCount;
            texDesc.format = depthFormat;
            texDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
            GFXERRCHECK(gfxDeviceCreateTexture(m_device, &texDesc, &m_msaaDepthTexture));

            GfxTextureViewDescriptor viewDesc{};
            viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
            viewDesc.label = "HeadlessMsaaDepthView";
            viewDesc.viewType = viewType;
            viewDesc.format = depthFormat;
            viewDesc.baseMipLevel = 0;
            viewDesc.mipLevelCount = 1;
            viewDesc.baseArrayLayer = 0;
            viewDesc.arrayLayerCount = m_viewCount;
            GFXERRCHECK(gfxTextureCreateView(m_msaaDepthTexture, &viewDesc, &m_msaaDepthView));
        }
    }

    const uint32_t imageCount = m_frameIndex.GetCount();
    m_swapchainBuffers.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i) {
        auto& sb = m_swapchainBuffers[i];

        // Per-frame color texture (resolve target if MSAA, otherwise primary)
        {
            GfxTextureDescriptor texDesc{};
            texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
            texDesc.label = "HeadlessColor";
            texDesc.type = GFX_TEXTURE_TYPE_2D;
            texDesc.size = { m_extent.width, m_extent.height, 1 };
            texDesc.arrayLayerCount = m_viewCount;
            texDesc.mipLevelCount = 1;
            texDesc.sampleCount = GFX_SAMPLE_COUNT_1;
            texDesc.format = colorFormat;
            texDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
            GFXERRCHECK(gfxDeviceCreateTexture(m_device, &texDesc, &sb.colorTexture));

            GfxTextureViewDescriptor viewDesc{};
            viewDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_VIEW_DESCRIPTOR;
            viewDesc.label = "HeadlessColorView";
            viewDesc.viewType = viewType;
            viewDesc.format = colorFormat;
            viewDesc.baseMipLevel = 0;
            viewDesc.mipLevelCount = 1;
            viewDesc.baseArrayLayer = 0;
            viewDesc.arrayLayerCount = m_viewCount;
            GFXERRCHECK(gfxTextureCreateView(sb.colorTexture, &viewDesc, &sb.colorView));
        }

        GfxFramebufferAttachment colorAttachment{};
        GfxFramebufferAttachment depthAttachment{};
        if (sampleCount > GFX_SAMPLE_COUNT_1) {
            colorAttachment.view = m_msaaColorView;
            colorAttachment.resolveTarget = sb.colorView;
            depthAttachment.view = m_msaaDepthView;
            depthAttachment.resolveTarget = nullptr;
        } else {
            colorAttachment.view = sb.colorView;
            colorAttachment.resolveTarget = nullptr;
            depthAttachment.view = m_depthView;
            depthAttachment.resolveTarget = nullptr;
        }

        GfxFramebufferDescriptor fbDesc{};
        fbDesc.sType = GFX_STRUCTURE_TYPE_FRAMEBUFFER_DESCRIPTOR;
        fbDesc.label = "HeadlessFramebuffer";
        fbDesc.renderPass = m_renderPass;
        fbDesc.colorAttachments = &colorAttachment;
        fbDesc.colorAttachmentCount = 1;
        fbDesc.depthStencilAttachment = depthAttachment;
        fbDesc.extent = m_extent;
        GFXERRCHECK(gfxDeviceCreateFramebuffer(m_device, &fbDesc, &sb.framebuffer));

        GfxCommandEncoderDescriptor ceDesc{};
        ceDesc.sType = GFX_STRUCTURE_TYPE_COMMAND_ENCODER_DESCRIPTOR;
        ceDesc.label = "HeadlessCommandEncoder";
        GFXERRCHECK(gfxDeviceCreateCommandEncoder(m_device, &ceDesc, &sb.commandEncoder));

        GfxFenceDescriptor fenceDesc{};
        fenceDesc.sType = GFX_STRUCTURE_TYPE_FENCE_DESCRIPTOR;
        fenceDesc.label = "HeadlessFence";
        fenceDesc.signaled = true;
        GFXERRCHECK(gfxDeviceCreateFence(m_device, &fenceDesc, &sb.fence));
    }
}

void HeadlessSwapchain::DestroyResources()
{
    for (auto& sb : m_swapchainBuffers) {
        if (sb.fence) {
            gfxFenceDestroy(sb.fence);
            sb.fence = nullptr;
        }
        if (sb.commandEncoder) {
            gfxCommandEncoderDestroy(sb.commandEncoder);
            sb.commandEncoder = nullptr;
        }
        if (sb.framebuffer) {
            gfxFramebufferDestroy(sb.framebuffer);
            sb.framebuffer = nullptr;
        }
        if (sb.colorView) {
            gfxTextureViewDestroy(sb.colorView);
            sb.colorView = nullptr;
        }
        if (sb.colorTexture) {
            gfxTextureDestroy(sb.colorTexture);
            sb.colorTexture = nullptr;
        }
    }
    m_swapchainBuffers.clear();

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
    if (m_depthView) {
        gfxTextureViewDestroy(m_depthView);
        m_depthView = nullptr;
    }
    if (m_depthTexture) {
        gfxTextureDestroy(m_depthTexture);
        m_depthTexture = nullptr;
    }
}

bool HeadlessSwapchain::BeginFrame(FrameContext& outContext)
{
    ASSERT(!m_isAcquired, "HeadlessSwapchain: previous frame not ended");

    auto& sb = m_swapchainBuffers[m_frameIndex];

    GFXERRCHECK(gfxFenceWait(sb.fence, UINT64_MAX));
    GFXERRCHECK(gfxFenceReset(sb.fence));

    m_isAcquired = true;

    GFXERRCHECK(gfxCommandEncoderBegin(sb.commandEncoder));

    outContext.frameBuffer = sb.framebuffer;
    outContext.commandEncoder = sb.commandEncoder;
    outContext.index = m_frameIndex;
    return true;
}

void HeadlessSwapchain::EndFrame()
{
    ASSERT(m_isAcquired, "HeadlessSwapchain: BeginFrame must succeed before EndFrame");

    auto& sb = m_swapchainBuffers[m_frameIndex];

    GFXERRCHECK(gfxCommandEncoderEnd(sb.commandEncoder));

    GfxCommandEncoder encoders[] = { sb.commandEncoder };

    GfxSubmitDescriptor submitDesc{};
    submitDesc.sType = GFX_STRUCTURE_TYPE_SUBMIT_DESCRIPTOR;
    submitDesc.commandEncoders = encoders;
    submitDesc.commandEncoderCount = 1;
    submitDesc.signalFence = sb.fence;
    GFXERRCHECK(m_graphicsQueue.Submit(&submitDesc));

    ++m_frameIndex;
    m_isAcquired = false;
}

GfxExtent2D HeadlessSwapchain::GetExtent() const
{
    return m_extent;
}

uint32_t HeadlessSwapchain::GetImageCount() const
{
    return m_frameIndex.GetCount();
}

void HeadlessSwapchain::Print() const
{
    LOGI("HeadlessSwapchain:");
    LOGI("\tExtent = %u x %u", m_extent.width, m_extent.height);
    LOGI("\tImages = %u", m_frameIndex.GetCount());
}
} // namespace prev::render::swapchain::headless

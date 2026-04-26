#include "PresentableSwapchain.h"

#include "../../../common/Logger.h"

#include <vector>

namespace prev::render::swapchain::presentable {
PresentableSwapchain::PresentableSwapchain(core::device::Device& device, pass::RenderPass& renderPass, GfxSurface surface, GfxExtent2D extent, GfxPresentMode presentMode, uint32_t imageCount, uint32_t viewCount, uint32_t maxFramesInFlight)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_graphicsQueue{ device.GetQueue(core::device::QueueType::GRAPHICS) }
    , m_presentQueue{ device.GetQueue(core::device::QueueType::PRESENT) }
    , m_viewCount{ viewCount }
    , m_maxFramesInFlight{ maxFramesInFlight }
{
    GfxSwapchainDescriptor swapchainDesc{};
    swapchainDesc.sType = GFX_STRUCTURE_TYPE_SWAPCHAIN_DESCRIPTOR;
    swapchainDesc.pNext = nullptr;
    swapchainDesc.label = "PresentableSwapchain";
    swapchainDesc.surface = surface;
    swapchainDesc.extent = extent;
    swapchainDesc.format = m_renderPass.GetColorFormat();
    swapchainDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
    swapchainDesc.presentMode = presentMode;
    swapchainDesc.imageCount = imageCount;
    GFXERRCHECK(gfxDeviceCreateSwapchain(m_device, &swapchainDesc, &m_swapchain));

    GfxSwapchainInfo info{};
    GFXERRCHECK(gfxSwapchainGetInfo(m_swapchain, &info));
    m_extent = info.extent;

    const uint32_t actualImageCount = info.imageCount;
    const uint32_t framesInFlight = (m_maxFramesInFlight > 0) ? m_maxFramesInFlight : actualImageCount;
    m_frameIndex = util::CircularIndex<uint32_t>{ framesInFlight };

    CreateResources();

    LOGI("PresentableSwapchain created: %ux%u, %u images, %u frames-in-flight",
        m_extent.width, m_extent.height, actualImageCount, framesInFlight);
}

PresentableSwapchain::~PresentableSwapchain()
{
    m_device.WaitIdle();
    DestroyResources();
    if (m_swapchain) {
        gfxSwapchainDestroy(m_swapchain);
    }
    LOGI("PresentableSwapchain destroyed");
}

void PresentableSwapchain::CreateResources()
{
    const GfxFormat colorFormat = m_renderPass.GetColorFormat();
    const GfxFormat depthFormat = m_renderPass.GetDepthFormat();
    const GfxSampleCount sampleCount = m_renderPass.GetSampleCount();
    const GfxTextureViewType viewType = (m_viewCount > 1) ? GFX_TEXTURE_VIEW_TYPE_2D_ARRAY : GFX_TEXTURE_VIEW_TYPE_2D;

    // Depth texture
    {
        GfxTextureDescriptor texDesc{};
        texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
        texDesc.label = "PresentableDepth";
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
        viewDesc.label = "PresentableDepthView";
        viewDesc.viewType = viewType;
        viewDesc.format = depthFormat;
        viewDesc.baseMipLevel = 0;
        viewDesc.mipLevelCount = 1;
        viewDesc.baseArrayLayer = 0;
        viewDesc.arrayLayerCount = m_viewCount;
        GFXERRCHECK(gfxTextureCreateView(m_depthTexture, &viewDesc, &m_depthView));
    }

    if (sampleCount > GFX_SAMPLE_COUNT_1) {
        // MSAA color texture
        {
            GfxTextureDescriptor texDesc{};
            texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
            texDesc.label = "PresentableMsaaColor";
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
            viewDesc.label = "PresentableMsaaColorView";
            viewDesc.viewType = viewType;
            viewDesc.format = colorFormat;
            viewDesc.baseMipLevel = 0;
            viewDesc.mipLevelCount = 1;
            viewDesc.baseArrayLayer = 0;
            viewDesc.arrayLayerCount = m_viewCount;
            GFXERRCHECK(gfxTextureCreateView(m_msaaColorTexture, &viewDesc, &m_msaaColorView));
        }

        // MSAA depth texture
        {
            GfxTextureDescriptor texDesc{};
            texDesc.sType = GFX_STRUCTURE_TYPE_TEXTURE_DESCRIPTOR;
            texDesc.label = "PresentableMsaaDepth";
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
            viewDesc.label = "PresentableMsaaDepthView";
            viewDesc.viewType = viewType;
            viewDesc.format = depthFormat;
            viewDesc.baseMipLevel = 0;
            viewDesc.mipLevelCount = 1;
            viewDesc.baseArrayLayer = 0;
            viewDesc.arrayLayerCount = m_viewCount;
            GFXERRCHECK(gfxTextureCreateView(m_msaaDepthTexture, &viewDesc, &m_msaaDepthView));
        }
    }

    // Per-swapchain-image resources
    GfxSwapchainInfo info{};
    GFXERRCHECK(gfxSwapchainGetInfo(m_swapchain, &info));
    const uint32_t swapchainImageCount = info.imageCount;

    m_swapchainBuffers.resize(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        auto& sb = m_swapchainBuffers[i];

        // View is owned by the swapchain, do not destroy it
        GFXERRCHECK(gfxSwapchainGetTextureView(m_swapchain, i, &sb.view));

        GfxFramebufferAttachment colorAttachment{};
        GfxFramebufferAttachment depthAttachment{};
        if (sampleCount > GFX_SAMPLE_COUNT_1) {
            colorAttachment.view = m_msaaColorView;
            colorAttachment.resolveTarget = sb.view;
            depthAttachment.view = m_msaaDepthView;
            depthAttachment.resolveTarget = nullptr;
        } else {
            colorAttachment.view = sb.view;
            colorAttachment.resolveTarget = nullptr;
            depthAttachment.view = m_depthView;
            depthAttachment.resolveTarget = nullptr;
        }

        GfxFramebufferDescriptor fbDesc{};
        fbDesc.sType = GFX_STRUCTURE_TYPE_FRAMEBUFFER_DESCRIPTOR;
        fbDesc.label = "PresentableFramebuffer";
        fbDesc.renderPass = m_renderPass;
        fbDesc.colorAttachments = &colorAttachment;
        fbDesc.colorAttachmentCount = 1;
        fbDesc.depthStencilAttachment = depthAttachment;
        fbDesc.extent = m_extent;
        GFXERRCHECK(gfxDeviceCreateFramebuffer(m_device, &fbDesc, &sb.framebuffer));

        GfxSemaphoreDescriptor semDesc{};
        semDesc.sType = GFX_STRUCTURE_TYPE_SEMAPHORE_DESCRIPTOR;
        semDesc.label = "RenderSemaphore";
        semDesc.type = GFX_SEMAPHORE_TYPE_BINARY;
        semDesc.initialValue = 0;
        GFXERRCHECK(gfxDeviceCreateSemaphore(m_device, &semDesc, &sb.renderSemaphore));
    }

    // Per-frame-in-flight resources
    const uint32_t framesInFlight = m_frameIndex.GetCount();
    m_framesInFlight.resize(framesInFlight);
    for (uint32_t i = 0; i < framesInFlight; ++i) {
        auto& f = m_framesInFlight[i];

        GfxCommandEncoderDescriptor ceDesc{};
        ceDesc.sType = GFX_STRUCTURE_TYPE_COMMAND_ENCODER_DESCRIPTOR;
        ceDesc.label = "SwapchainCommandEncoder";
        GFXERRCHECK(gfxDeviceCreateCommandEncoder(m_device, &ceDesc, &f.commandEncoder));

        GfxSemaphoreDescriptor semDesc{};
        semDesc.sType = GFX_STRUCTURE_TYPE_SEMAPHORE_DESCRIPTOR;
        semDesc.label = "AcquireSemaphore";
        semDesc.type = GFX_SEMAPHORE_TYPE_BINARY;
        semDesc.initialValue = 0;
        GFXERRCHECK(gfxDeviceCreateSemaphore(m_device, &semDesc, &f.acquireSemaphore));

        GfxFenceDescriptor fenceDesc{};
        fenceDesc.sType = GFX_STRUCTURE_TYPE_FENCE_DESCRIPTOR;
        fenceDesc.label = "FrameFence";
        fenceDesc.signaled = true;
        GFXERRCHECK(gfxDeviceCreateFence(m_device, &fenceDesc, &f.fence));
    }
}

void PresentableSwapchain::DestroyResources()
{
    for (auto& sb : m_swapchainBuffers) {
        if (sb.renderSemaphore) {
            gfxSemaphoreDestroy(sb.renderSemaphore);
            sb.renderSemaphore = nullptr;
        }
        if (sb.framebuffer) {
            gfxFramebufferDestroy(sb.framebuffer);
            sb.framebuffer = nullptr;
        }
        // sb.view is owned by the swapchain, do not destroy
    }
    m_swapchainBuffers.clear();

    for (auto& f : m_framesInFlight) {
        if (f.fence) {
            gfxFenceDestroy(f.fence);
            f.fence = nullptr;
        }
        if (f.acquireSemaphore) {
            gfxSemaphoreDestroy(f.acquireSemaphore);
            f.acquireSemaphore = nullptr;
        }
        if (f.commandEncoder) {
            gfxCommandEncoderDestroy(f.commandEncoder);
            f.commandEncoder = nullptr;
        }
    }
    m_framesInFlight.clear();

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

bool PresentableSwapchain::BeginFrame(FrameContext& outContext)
{
    ASSERT(!m_isAcquired, "PresentableSwapchain: previous frame not ended");

    auto& frameInFlight = m_framesInFlight[m_frameIndex];

    GFXERRCHECK(gfxFenceWait(frameInFlight.fence, UINT64_MAX));
    GFXERRCHECK(gfxFenceReset(frameInFlight.fence));

    uint32_t acquireIndex{};
    const GfxResult result = gfxSwapchainAcquireNextImage(m_swapchain, UINT64_MAX, frameInFlight.acquireSemaphore, nullptr, &acquireIndex);
    if (result == GFX_RESULT_ERROR_OUT_OF_DATE || result == GFX_RESULT_ERROR_SURFACE_LOST) {
        return false;
    }
    GFXERRCHECK(result);

    m_acquiredIndex = acquireIndex;
    m_isAcquired = true;

    GFXERRCHECK(gfxCommandEncoderBegin(frameInFlight.commandEncoder));

    outContext.frameBuffer = m_swapchainBuffers[m_acquiredIndex].framebuffer;
    outContext.commandEncoder = frameInFlight.commandEncoder;
    outContext.index = m_frameIndex;
    return true;
}

void PresentableSwapchain::EndFrame()
{
    ASSERT(m_isAcquired, "PresentableSwapchain: BeginFrame must succeed before EndFrame");

    auto& frameInFlight = m_framesInFlight[m_frameIndex];
    auto& swapchainBuffer = m_swapchainBuffers[m_acquiredIndex];

    GFXERRCHECK(gfxCommandEncoderEnd(frameInFlight.commandEncoder));

    GfxCommandEncoder encoders[] = { frameInFlight.commandEncoder };
    GfxSemaphore waitSems[] = { frameInFlight.acquireSemaphore };
    GfxSemaphore signalSems[] = { swapchainBuffer.renderSemaphore };

    GfxSubmitDescriptor submitDesc{};
    submitDesc.sType = GFX_STRUCTURE_TYPE_SUBMIT_DESCRIPTOR;
    submitDesc.commandEncoders = encoders;
    submitDesc.commandEncoderCount = 1;
    submitDesc.waitSemaphores = waitSems;
    submitDesc.waitSemaphoreCount = 1;
    submitDesc.signalSemaphores = signalSems;
    submitDesc.signalSemaphoreCount = 1;
    submitDesc.signalFence = frameInFlight.fence;
    GFXERRCHECK(m_graphicsQueue.Submit(&submitDesc));

    GfxSemaphore presentWaitSems[] = { swapchainBuffer.renderSemaphore };
    GfxPresentDescriptor presentDesc{};
    presentDesc.sType = GFX_STRUCTURE_TYPE_PRESENT_DESCRIPTOR;
    presentDesc.waitSemaphores = presentWaitSems;
    presentDesc.waitSemaphoreCount = 1;
    // Ignore present result — OUT_OF_DATE is handled on next BeginFrame
    gfxSwapchainPresent(m_swapchain, &presentDesc);

    ++m_frameIndex;
    m_isAcquired = false;
}

GfxExtent2D PresentableSwapchain::GetExtent() const
{
    return m_extent;
}

uint32_t PresentableSwapchain::GetImageCount() const
{
    return static_cast<uint32_t>(m_swapchainBuffers.size());
}

void PresentableSwapchain::Print() const
{
    LOGI("PresentableSwapchain:");
    LOGI("\tExtent          = %u x %u", m_extent.width, m_extent.height);
    LOGI("\tImages          = %u", static_cast<uint32_t>(m_swapchainBuffers.size()));
    LOGI("\tFrames-in-flight= %u", m_frameIndex.GetCount());
}
} // namespace prev::render::swapchain::presentable

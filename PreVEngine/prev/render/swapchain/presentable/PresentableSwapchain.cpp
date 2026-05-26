#include "PresentableSwapchain.h"

#include "../../buffer/ImageBufferBuilder.h"
#include "../../framebuffer/FramebufferBuilder.h"

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

    const auto& graphicsQueue = m_device.GetQueue(core::device::QueueType::GRAPHICS);

    // Depth texture
    m_depthBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, graphicsQueue }
                        .SetExtent({ m_extent.width, m_extent.height, 1 })
                        .SetFormat(depthFormat)
                        .SetType(GFX_TEXTURE_TYPE_2D)
                        .SetViewType(viewType)
                        .SetSampleCount(sampleCount)
                        .SetLayerCount(m_viewCount)
                        .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT)
                        .Build();

    if (sampleCount > GFX_SAMPLE_COUNT_1) {
        // MSAA color texture
        m_msaaColorBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, graphicsQueue }
                                .SetExtent({ m_extent.width, m_extent.height, 1 })
                                .SetFormat(colorFormat)
                                .SetType(GFX_TEXTURE_TYPE_2D)
                                .SetViewType(viewType)
                                .SetSampleCount(sampleCount)
                                .SetLayerCount(m_viewCount)
                                .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT)
                                .Build();

        // MSAA depth texture
        m_msaaDepthBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, graphicsQueue }
                                .SetExtent({ m_extent.width, m_extent.height, 1 })
                                .SetFormat(depthFormat)
                                .SetType(GFX_TEXTURE_TYPE_2D)
                                .SetViewType(viewType)
                                .SetSampleCount(sampleCount)
                                .SetLayerCount(m_viewCount)
                                .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT)
                                .Build();
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

        GfxTextureView colorView{};
        GfxTextureView depthView{};
        GfxTextureView colorResolve{};
        if (sampleCount > GFX_SAMPLE_COUNT_1) {
            colorView = m_msaaColorBuffer->GetTextureView();
            colorResolve = sb.view;
            depthView = m_msaaDepthBuffer->GetTextureView();
        } else {
            colorView = sb.view;
            depthView = m_depthBuffer->GetTextureView();
        }

        sb.framebuffer = prev::render::framebuffer::FramebufferBuilder{ m_device, m_renderPass }
                             .SetExtent(m_extent)
                             .AddColorAttachment(colorView, colorResolve)
                             .SetDepthStencilAttachment(depthView)
                             .Build();

        sb.renderSemaphore = std::make_unique<core::sync::Semaphore>(m_device, GFX_SEMAPHORE_TYPE_BINARY, 0, "RenderSemaphore");
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

        f.acquireSemaphore = std::make_unique<core::sync::Semaphore>(m_device, GFX_SEMAPHORE_TYPE_BINARY, 0, "AcquireSemaphore");
        f.fence = std::make_unique<core::sync::Fence>(m_device, true, "FrameFence");
    }
}

void PresentableSwapchain::DestroyResources()
{
    for (auto& sb : m_swapchainBuffers) {
        sb.renderSemaphore.reset();
        sb.framebuffer.reset();
        // sb.view is owned by the swapchain, do not destroy
    }
    m_swapchainBuffers.clear();

    for (auto& f : m_framesInFlight) {
        f.fence.reset();
        f.acquireSemaphore.reset();
        if (f.commandEncoder) {
            gfxCommandEncoderDestroy(f.commandEncoder);
            f.commandEncoder = nullptr;
        }
    }
    m_framesInFlight.clear();

    m_msaaDepthBuffer.reset();
    m_msaaColorBuffer.reset();
    m_depthBuffer.reset();
}

bool PresentableSwapchain::BeginFrame(FrameContext& outContext)
{
    ASSERT(!m_isAcquired, "PresentableSwapchain: previous frame not ended");

    auto& frameInFlight = m_framesInFlight[m_frameIndex];

    frameInFlight.fence->Wait();
    frameInFlight.fence->Reset();

    uint32_t acquireIndex{};
    const GfxResult result = gfxSwapchainAcquireNextImage(m_swapchain, UINT64_MAX, *frameInFlight.acquireSemaphore, nullptr, &acquireIndex);
    if (result == GFX_RESULT_ERROR_OUT_OF_DATE || result == GFX_RESULT_ERROR_SURFACE_LOST) {
        return false;
    }
    GFXERRCHECK(result);

    m_acquiredIndex = acquireIndex;
    m_isAcquired = true;

    GFXERRCHECK(gfxCommandEncoderBegin(frameInFlight.commandEncoder));

    outContext.frameBuffer = *m_swapchainBuffers[m_acquiredIndex].framebuffer;
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
    GfxSemaphore waitSems[] = { *frameInFlight.acquireSemaphore };
    GfxSemaphore signalSems[] = { *swapchainBuffer.renderSemaphore };

    GfxSubmitDescriptor submitDesc{};
    submitDesc.sType = GFX_STRUCTURE_TYPE_SUBMIT_DESCRIPTOR;
    submitDesc.commandEncoders = encoders;
    submitDesc.commandEncoderCount = 1;
    submitDesc.waitSemaphores = waitSems;
    submitDesc.waitSemaphoreCount = 1;
    submitDesc.signalSemaphores = signalSems;
    submitDesc.signalSemaphoreCount = 1;
    submitDesc.signalFence = *frameInFlight.fence;
    GFXERRCHECK(m_graphicsQueue.Submit(&submitDesc));

    GfxSemaphore presentWaitSems[] = { *swapchainBuffer.renderSemaphore };
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

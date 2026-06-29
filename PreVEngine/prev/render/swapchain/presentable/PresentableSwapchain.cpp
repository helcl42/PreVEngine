#include "PresentableSwapchain.h"

#include "../../buffer/ImageBufferBuilder.h"
#include "../../framebuffer/FramebufferBuilder.h"

#include "../../../common/Logger.h"

#include <algorithm>
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
    // Clamp the requested count to the surface's supported [min, max] image count.
    GfxSurfaceInfo surfaceInfo{};
    GFXERRCHECK(gfxSurfaceGetInfo(surface, m_device.GetGPU(), &surfaceInfo));
    const uint32_t clampedImageCount{ std::max(surfaceInfo.minImageCount, std::min(imageCount, surfaceInfo.maxImageCount > 0 ? surfaceInfo.maxImageCount : imageCount)) };

    GfxSwapchainDescriptor swapchainDesc{};
    swapchainDesc.sType = GFX_STRUCTURE_TYPE_SWAPCHAIN_DESCRIPTOR;
    swapchainDesc.pNext = nullptr;
    swapchainDesc.label = "PresentableSwapchain";
    swapchainDesc.surface = surface;
    swapchainDesc.extent = extent;
    swapchainDesc.format = m_renderPass.GetColorFormat();
    swapchainDesc.usage = GFX_TEXTURE_USAGE_RENDER_ATTACHMENT;
    swapchainDesc.presentMode = presentMode;
    swapchainDesc.imageCount = clampedImageCount;
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
    // Shared depth + MSAA attachments and per-image framebuffer assembly (the part identical across swapchains).
    m_targets = std::make_unique<SwapchainTargets>(m_device, m_renderPass, m_extent, m_renderPass.GetColorFormat(), m_renderPass.GetDepthFormat(), m_renderPass.GetSampleCount(), m_viewCount, /*createSharedDepth*/ true);

    // Per-swapchain-image resources
    GfxSwapchainInfo info{};
    GFXERRCHECK(gfxSwapchainGetInfo(m_swapchain, &info));
    const uint32_t swapchainImageCount = info.imageCount;

    m_swapchainBuffers.resize(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; ++i) {
        auto& sb = m_swapchainBuffers[i];

        // View is owned by the swapchain, do not destroy it
        GFXERRCHECK(gfxSwapchainGetTextureView(m_swapchain, i, &sb.view));

        sb.framebuffer = m_targets->CreateFramebuffer(sb.view);

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

    m_targets.reset();
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

void PresentableSwapchain::EndFrame(const FrameSubmitSync& submitSync)
{
    ASSERT(m_isAcquired, "PresentableSwapchain: BeginFrame must succeed before EndFrame");

    auto& frameInFlight = m_framesInFlight[m_frameIndex];
    auto& swapchainBuffer = m_swapchainBuffers[m_acquiredIndex];

    GFXERRCHECK(gfxCommandEncoderEnd(frameInFlight.commandEncoder));

    GfxCommandEncoder encoders[] = { frameInFlight.commandEncoder };

    // Wait: acquired image (binary, value 0) + this frame's deps. Values are index-aligned; gfx reads them
    // only for timeline semaphores.
    std::vector<GfxSemaphore> waitSems{ *frameInFlight.acquireSemaphore };
    std::vector<GfxPipelineStageFlags> waitStages{ GFX_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT };
    std::vector<uint64_t> waitValues{ 0 };
    for (const auto& wait : submitSync.waits) {
        waitSems.push_back(wait.semaphore);
        waitStages.push_back(wait.stage);
        waitValues.push_back(wait.value);
    }

    // Signal: render semaphore (binary, present waits on it) + this frame's deps.
    std::vector<GfxSemaphore> signalSems{ *swapchainBuffer.renderSemaphore };
    std::vector<uint64_t> signalValues{ 0 };
    for (const auto& signal : submitSync.signals) {
        signalSems.push_back(signal.semaphore);
        signalValues.push_back(signal.value);
    }

    GfxSubmitDescriptor submitDesc{};
    submitDesc.sType = GFX_STRUCTURE_TYPE_SUBMIT_DESCRIPTOR;
    submitDesc.commandEncoders = encoders;
    submitDesc.commandEncoderCount = 1;
    submitDesc.waitSemaphores = waitSems.data();
    submitDesc.waitStages = waitStages.data();
    submitDesc.waitValues = waitValues.data();
    submitDesc.waitSemaphoreCount = static_cast<uint32_t>(waitSems.size());
    submitDesc.signalSemaphores = signalSems.data();
    submitDesc.signalValues = signalValues.data();
    submitDesc.signalSemaphoreCount = static_cast<uint32_t>(signalSems.size());
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

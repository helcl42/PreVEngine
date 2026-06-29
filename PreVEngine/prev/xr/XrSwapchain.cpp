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

    // Color textures come pre-imported from OpenXR; depth comes from OpenXR or, if absent, a shared depth.
    const auto colorTextures = m_xr.GetColorImages();
    const auto swapchainImagesCount = static_cast<uint32_t>(colorTextures.size());
    const bool hasXrDepth = m_xr.HasDepthImages();
    const std::vector<GfxTexture> depthTextures{ hasXrDepth ? m_xr.GetDepthImages() : std::vector<GfxTexture>{} };

    // Shared MSAA color/depth + (when XR provides no depth) a shared depth, and per-image framebuffer assembly.
    m_targets = std::make_unique<prev::render::swapchain::SwapchainTargets>(m_device, m_renderPass, m_extent, colorFormat, depthFormat, m_sampleCount, viewCount, /*createSharedDepth*/ !hasXrDepth);

    m_swapchainBuffers.resize(swapchainImagesCount);
    for (uint32_t i = 0; i < swapchainImagesCount; ++i) {
        auto& sb = m_swapchainBuffers[i];

        // Per-image color view over the OpenXR-imported color texture.
        sb.colorTexture = colorTextures[i];
        sb.colorView = prev::render::buffer::ImageBufferViewBuilder{ sb.colorTexture, viewType, colorFormat, viewCount }
                           .Build();

        // Use the OpenXR-provided depth when present; otherwise null -> the framebuffer uses the shared depth.
        GfxTextureView depthView{};
        if (hasXrDepth) {
            sb.depthTexture = depthTextures[i];
            sb.depthView = prev::render::buffer::ImageBufferViewBuilder{ sb.depthTexture, viewType, depthFormat, viewCount }
                               .Build();
            depthView = *sb.depthView;
        }

        sb.framebuffer = m_targets->CreateFramebuffer(*sb.colorView, depthView);

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
        // colorTexture/depthTexture owned by IXr or the shared depth — don't destroy here
    }
    m_swapchainBuffers.clear();

    m_targets.reset();
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

void XrSwapchain::EndFrame(const prev::render::FrameSubmitSync& submitSync)
{
    auto& sb = m_swapchainBuffers[m_acquiredIndex];

    GFXERRCHECK(gfxCommandEncoderEnd(sb.commandEncoder));

    GfxCommandEncoder encoders[] = { sb.commandEncoder };

    // Wait on / signal any cross-submission deps from this frame's renderers (XR owns image sync separately).
    std::vector<GfxSemaphore> waitSems;
    std::vector<GfxPipelineStageFlags> waitStages;
    std::vector<uint64_t> waitValues;
    for (const auto& wait : submitSync.waits) {
        waitSems.push_back(wait.semaphore);
        waitStages.push_back(wait.stage);
        waitValues.push_back(wait.value);
    }
    std::vector<GfxSemaphore> signalSems;
    std::vector<uint64_t> signalValues;
    for (const auto& signal : submitSync.signals) {
        signalSems.push_back(signal.semaphore);
        signalValues.push_back(signal.value);
    }

    GfxSubmitDescriptor submitDesc{};
    submitDesc.sType = GFX_STRUCTURE_TYPE_SUBMIT_DESCRIPTOR;
    submitDesc.commandEncoders = encoders;
    submitDesc.commandEncoderCount = 1;
    submitDesc.waitSemaphores = waitSems.empty() ? nullptr : waitSems.data();
    submitDesc.waitStages = waitStages.empty() ? nullptr : waitStages.data();
    submitDesc.waitValues = waitValues.empty() ? nullptr : waitValues.data();
    submitDesc.waitSemaphoreCount = static_cast<uint32_t>(waitSems.size());
    submitDesc.signalSemaphores = signalSems.empty() ? nullptr : signalSems.data();
    submitDesc.signalValues = signalValues.empty() ? nullptr : signalValues.data();
    submitDesc.signalSemaphoreCount = static_cast<uint32_t>(signalSems.size());
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
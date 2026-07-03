#include "XrSwapchain.h"

#ifdef ENABLE_XR

#include "../render/buffer/ImageBufferBuilder.h"
#include "../render/buffer/ImageBufferViewBuilder.h"
#include "../render/framebuffer/FramebufferBuilder.h"

#include "../common/Logger.h"

#include <algorithm>

namespace prev::xr {
XrSwapchain::XrSwapchain(prev::core::device::Device& device, prev::render::pass::RenderPass& renderPass, xr::IXr& xr, GfxSampleCount sampleCount, uint32_t maxFramesInFlight)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_xr{ xr }
    , m_sampleCount{ sampleCount }
    , m_graphicsQueue{ m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
    , m_framesInFlight{ maxFramesInFlight > 0 ? maxFramesInFlight : 2 }
    , m_frameInFlightIndex{ m_framesInFlight - 1 } // pre-incremented each frame -> first frame lands on 0
{
    // Resources are built lazily in BeginFrame - images (and on WebXR the extent) exist only once frames arrive.
    LOGI("XR Swapchain created");
}

XrSwapchain::~XrSwapchain()
{
    m_device.WaitIdle();
    DestroyResources();
    LOGI("XR Swapchain destroyed");
}

bool XrSwapchain::IsMultiview() const
{
    return MAX_PER_PASS_VIEW_COUNT_VALUE > 1;
}

void XrSwapchain::CreateResources(const GfxExtent2D& extent)
{
    m_extent = extent;
    const bool hasXrDepth = m_xr.HasDepthImages();
    const uint32_t targetLayers = IsMultiview() ? m_xr.GetViewCount() : 1;
    m_targets = std::make_unique<prev::render::swapchain::SwapchainTargets>(m_device, m_renderPass, m_extent, m_xr.GetColorFormat(), m_xr.GetDepthFormat(), m_sampleCount, targetLayers, /*createSharedDepth*/ !hasXrDepth);

    m_swapchainBuffers.resize(std::max(m_xr.GetImageCount(), 1u));
    for (auto& sb : m_swapchainBuffers) {
        CreateBufferCommands(sb);
    }
}

void XrSwapchain::DestroyResources()
{
    for (auto& sb : m_swapchainBuffers) {
        DestroyBufferCommands(sb);
        DestroySlotTargets(sb);
    }
    m_swapchainBuffers.clear();

    m_targets.reset();
}

void XrSwapchain::RecreateResources(const GfxExtent2D& extent)
{
    m_device.WaitIdle(); // targets may still be referenced by in-flight frames
    DestroyResources();
    CreateResources(extent);
}

void XrSwapchain::BuildSlotTargets(SwapchainBuffer& sb, GfxTexture colorTexture, GfxTexture depthTexture)
{
    DestroySlotTargets(sb);

    sb.colorTexture = colorTexture;
    sb.depthTexture = depthTexture;

    const uint32_t viewCount = m_xr.GetViewCount();
    if (IsMultiview()) {
        const GfxTextureViewType viewType = GFX_TEXTURE_VIEW_TYPE_2D_ARRAY;
        sb.multiviewTarget.colorView = prev::render::buffer::ImageBufferViewBuilder{ colorTexture, viewType, m_xr.GetColorFormat(), viewCount }
                                           .Build();
        GfxTextureView depthView{};
        if (depthTexture) {
            sb.multiviewTarget.depthView = prev::render::buffer::ImageBufferViewBuilder{ depthTexture, viewType, m_xr.GetDepthFormat(), viewCount }
                                               .Build();
            depthView = *sb.multiviewTarget.depthView;
        }
        sb.multiviewTarget.framebuffer = m_targets->CreateFramebuffer(*sb.multiviewTarget.colorView, depthView);
    } else {
        // All eyes built up front - every eye's views must stay alive for the frame's single command buffer.
        sb.perEyeTargets.resize(viewCount);
        for (uint32_t layer = 0; layer < viewCount; ++layer) {
            sb.perEyeTargets[layer] = BuildLayerTarget(colorTexture, depthTexture, layer);
        }
    }
}

bool XrSwapchain::HasBuiltTargets(const SwapchainBuffer& sb) const
{
    return IsMultiview() ? (sb.multiviewTarget.framebuffer != nullptr) : !sb.perEyeTargets.empty();
}

void XrSwapchain::DestroySlotTargets(SwapchainBuffer& sb)
{
    // Reset in dependency order: framebuffers reference the views.
    sb.perEyeTargets.clear();
    sb.multiviewTarget.framebuffer.reset();
    sb.multiviewTarget.depthView.reset();
    sb.multiviewTarget.colorView.reset();
}

XrSwapchain::RenderTarget XrSwapchain::BuildLayerTarget(GfxTexture colorTexture, GfxTexture depthTexture, uint32_t layer)
{
    RenderTarget target{};
    target.colorView = prev::render::buffer::ImageBufferViewBuilder{ colorTexture, GFX_TEXTURE_VIEW_TYPE_2D, m_xr.GetColorFormat(), 1 }
                           .SetBaseArrayLayer(layer)
                           .Build();
    GfxTextureView depthView{};
    if (depthTexture) {
        target.depthView = prev::render::buffer::ImageBufferViewBuilder{ depthTexture, GFX_TEXTURE_VIEW_TYPE_2D, m_xr.GetDepthFormat(), 1 }
                               .SetBaseArrayLayer(layer)
                               .Build();
        depthView = *target.depthView;
    }
    target.framebuffer = m_targets->CreateFramebuffer(*target.colorView, depthView);
    return target;
}

void XrSwapchain::CreateBufferCommands(SwapchainBuffer& sb)
{
    GfxCommandEncoderDescriptor ceDesc{};
    ceDesc.sType = GFX_STRUCTURE_TYPE_COMMAND_ENCODER_DESCRIPTOR;
    ceDesc.label = "XrCommandEncoder";
    GFXERRCHECK(gfxDeviceCreateCommandEncoder(m_device, &ceDesc, &sb.commandEncoder));

    sb.fence = std::make_unique<prev::core::sync::Fence>(m_device, true, "XrFence");
}

void XrSwapchain::DestroyBufferCommands(SwapchainBuffer& sb)
{
    sb.fence.reset();
    if (sb.commandEncoder) {
        gfxCommandEncoderDestroy(sb.commandEncoder);
        sb.commandEncoder = {};
    }
}

bool XrSwapchain::BeginFrame(prev::render::swapchain::FrameContext& outContext)
{
    XrFrameImages frameImages{};
    if (!m_xr.GetFrameImages(frameImages)) {
        return false; // no XR frame acquired (yet)
    }

    const GfxExtent2D extent = m_xr.GetExtent();
    if (!m_targets || extent.width != m_extent.width || extent.height != m_extent.height) {
        RecreateResources(extent);
    }

    m_acquiredIndex = frameImages.imageIndex;
    m_imagesChanged = frameImages.imagesChanged;
    ASSERT(m_acquiredIndex < m_swapchainBuffers.size(), "XrSwapchain: acquired image index out of range");

    auto& sb = m_swapchainBuffers[m_acquiredIndex];
    if (frameImages.imagesChanged || !HasBuiltTargets(sb)) {
        BuildSlotTargets(sb, frameImages.colorImage, frameImages.depthImage);
    }

    if (frameImages.imagesChanged) {
        // Per-frame images (WebXR/WebGPU): no synchronous fence wait exists; rotate the in-flight slot -
        // safe unfenced because submits are queue-ordered and resources live until their submission completes.
        m_frameInFlightIndex = (m_frameInFlightIndex + 1) % m_framesInFlight;
        outContext.index = m_frameInFlightIndex;
    } else {
        // Persistent images (OpenXR): fence-pace this slot before re-recording its encoder.
        sb.fence->Wait();
        sb.fence->Reset();
        outContext.index = m_acquiredIndex;
    }

    GFXERRCHECK(gfxCommandEncoderBegin(sb.commandEncoder));
    outContext.commandEncoder = sb.commandEncoder;
    return true;
}

void XrSwapchain::BeginPass(prev::render::swapchain::FrameContext& outContext, uint32_t passIndex)
{
    ASSERT(!m_passActive, "XrSwapchain: BeginPass called before the previous pass ended");
    m_passActive = true;

    auto& sb = m_swapchainBuffers[m_acquiredIndex];
    if (IsMultiview()) {
        outContext.frameBuffer = *sb.multiviewTarget.framebuffer;
        outContext.viewOffset = 0;
        outContext.viewCount = m_xr.GetViewCount();
    } else {
        outContext.frameBuffer = *sb.perEyeTargets[passIndex].framebuffer;
        outContext.viewOffset = passIndex;
        outContext.viewCount = 1;
    }
}

void XrSwapchain::EndPass(uint32_t passIndex)
{
    (void)passIndex;
    ASSERT(m_passActive, "XrSwapchain: EndPass called without a matching BeginPass");
    m_passActive = false;
}

void XrSwapchain::EndFrame(const prev::render::FrameSubmitSync& submitSync)
{
    ASSERT(!m_passActive, "XrSwapchain: EndFrame called with a pass still open");

    auto& sb = m_swapchainBuffers[m_acquiredIndex];

    GFXERRCHECK(gfxCommandEncoderEnd(sb.commandEncoder));

    GfxCommandEncoder encoders[] = { sb.commandEncoder };

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
    // Per-frame-image backends submit unfenced (see BeginFrame).
    submitDesc.signalFence = m_imagesChanged ? GfxFence{} : static_cast<GfxFence>(*sb.fence);
    GFXERRCHECK(m_graphicsQueue.Submit(&submitDesc));
}

GfxExtent2D XrSwapchain::GetExtent() const
{
    // Live extent until the first frame builds targets - consumers must never see a transient 0x0.
    return m_targets ? m_extent : m_xr.GetExtent();
}

uint32_t XrSwapchain::GetImageCount() const
{
    // Upper bound of FrameContext.index values; over-reporting is safe, under-reporting is not.
    return std::max(m_xr.GetImageCount(), m_framesInFlight);
}

uint32_t XrSwapchain::GetPassCount() const
{
    return IsMultiview() ? 1 : m_xr.GetViewCount();
}

void XrSwapchain::Print() const
{
    LOGI("XR Swapchain:");
    const GfxExtent2D extent = GetExtent();
    LOGI("\tExtent  = %u x %u", extent.width, extent.height);
    LOGI("\tImages  = %u", GetImageCount());
    LOGI("\tViews   = %u", m_xr.GetViewCount());
    LOGI("\tMultiview = %s", IsMultiview() ? "true" : "false");
}
} // namespace prev::xr

#endif

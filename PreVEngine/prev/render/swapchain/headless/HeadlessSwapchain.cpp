#include "HeadlessSwapchain.h"

#include "../../buffer/ImageBufferBuilder.h"
#include "../../framebuffer/FramebufferBuilder.h"

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

    // Shared depth + MSAA attachments and per-image framebuffer assembly (identical across swapchains).
    m_targets = std::make_unique<SwapchainTargets>(m_device, m_renderPass, m_extent, colorFormat, m_renderPass.GetDepthFormat(), m_renderPass.GetSampleCount(), m_viewCount, /*createSharedDepth*/ true);

    const uint32_t imageCount = m_frameIndex.GetCount();
    m_swapchainBuffers.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i) {
        auto& sb = m_swapchainBuffers[i];

        // Headless owns its color image (single-sample resolve/primary target the framebuffer renders into).
        sb.colorBuffer = CreateRenderAttachment(m_device, m_extent, colorFormat, GFX_SAMPLE_COUNT_1, m_viewCount);

        sb.framebuffer = m_targets->CreateFramebuffer(sb.colorBuffer->GetTextureView());

        GfxCommandEncoderDescriptor ceDesc{};
        ceDesc.sType = GFX_STRUCTURE_TYPE_COMMAND_ENCODER_DESCRIPTOR;
        ceDesc.label = "HeadlessCommandEncoder";
        GFXERRCHECK(gfxDeviceCreateCommandEncoder(m_device, &ceDesc, &sb.commandEncoder));

        sb.fence = std::make_unique<core::sync::Fence>(m_device, true, "HeadlessFence");
    }
}

void HeadlessSwapchain::DestroyResources()
{
    for (auto& sb : m_swapchainBuffers) {
        sb.fence.reset();
        if (sb.commandEncoder) {
            gfxCommandEncoderDestroy(sb.commandEncoder);
            sb.commandEncoder = nullptr;
        }
        sb.framebuffer.reset();
        sb.colorBuffer.reset();
    }
    m_swapchainBuffers.clear();

    m_targets.reset();
}

bool HeadlessSwapchain::BeginFrame(FrameContext& outContext)
{
    ASSERT(!m_isAcquired, "HeadlessSwapchain: previous frame not ended");

    auto& sb = m_swapchainBuffers[m_frameIndex];

    sb.fence->Wait();
    sb.fence->Reset();

    m_isAcquired = true;

    GFXERRCHECK(gfxCommandEncoderBegin(sb.commandEncoder));

    outContext.frameBuffer = *sb.framebuffer;
    outContext.commandEncoder = sb.commandEncoder;
    outContext.index = m_frameIndex;
    return true;
}

void HeadlessSwapchain::EndFrame(const FrameSubmitSync& submitSync)
{
    ASSERT(m_isAcquired, "HeadlessSwapchain: BeginFrame must succeed before EndFrame");

    auto& sb = m_swapchainBuffers[m_frameIndex];

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
    submitDesc.signalFence = *sb.fence;
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

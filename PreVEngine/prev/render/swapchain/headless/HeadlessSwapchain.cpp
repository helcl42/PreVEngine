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
    const GfxFormat depthFormat = m_renderPass.GetDepthFormat();
    const GfxSampleCount sampleCount = m_renderPass.GetSampleCount();
    const GfxTextureViewType viewType = (m_viewCount > 1) ? GFX_TEXTURE_VIEW_TYPE_2D_ARRAY : GFX_TEXTURE_VIEW_TYPE_2D;

    const auto& graphicsQueue = m_device.GetQueue(core::device::QueueType::GRAPHICS);

    // Shared depth texture
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
        // MSAA color
        m_msaaColorBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, graphicsQueue }
                                .SetExtent({ m_extent.width, m_extent.height, 1 })
                                .SetFormat(colorFormat)
                                .SetType(GFX_TEXTURE_TYPE_2D)
                                .SetViewType(viewType)
                                .SetSampleCount(sampleCount)
                                .SetLayerCount(m_viewCount)
                                .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT)
                                .Build();

        // MSAA depth
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

    const uint32_t imageCount = m_frameIndex.GetCount();
    m_swapchainBuffers.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; ++i) {
        auto& sb = m_swapchainBuffers[i];

        // Per-frame color texture (resolve target if MSAA, otherwise primary)
        sb.colorBuffer = prev::render::buffer::ImageBufferBuilder{ m_device, graphicsQueue }
                             .SetExtent({ m_extent.width, m_extent.height, 1 })
                             .SetFormat(colorFormat)
                             .SetType(GFX_TEXTURE_TYPE_2D)
                             .SetViewType(viewType)
                             .SetSampleCount(GFX_SAMPLE_COUNT_1)
                             .SetLayerCount(m_viewCount)
                             .SetUsageFlags(GFX_TEXTURE_USAGE_RENDER_ATTACHMENT)
                             .Build();

        GfxTextureView colorView{};
        GfxTextureView depthView{};
        GfxTextureView colorResolve{};
        if (sampleCount > GFX_SAMPLE_COUNT_1) {
            colorView = m_msaaColorBuffer->GetTextureView();
            colorResolve = sb.colorBuffer->GetTextureView();
            depthView = m_msaaDepthBuffer->GetTextureView();
        } else {
            colorView = sb.colorBuffer->GetTextureView();
            depthView = m_depthBuffer->GetTextureView();
        }

        sb.framebuffer = prev::render::framebuffer::FramebufferBuilder{ m_device, m_renderPass }
                             .SetExtent(m_extent)
                             .AddColorAttachment(colorView, colorResolve)
                             .SetDepthStencilAttachment(depthView)
                             .Build();

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

    m_msaaDepthBuffer.reset();
    m_msaaColorBuffer.reset();
    m_depthBuffer.reset();
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

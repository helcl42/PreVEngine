#include "HeadlessSwapchain.h"

#include "../../buffer/ImageBufferBuilder.h"

#include "../../../util/VkUtils.h"

namespace prev::render::swapchain::headless {
HeadlessSwapchain::HeadlessSwapchain(core::device::Device& device, core::memory::Allocator& allocator, pass::RenderPass& renderPass, VkSampleCountFlagBits sampleCount, uint32_t viewCount)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_sampleCount{ sampleCount }
    , m_viewCount{ viewCount }
    , m_graphicsQueue{ m_device.GetQueue(core::device::QueueType::GRAPHICS) }
    , m_extent{ 512, 512 }
    , m_imageCount{ 3 }
    , m_isAcquired{ false }
{
    m_commandPool = prev::util::vk::CreateCommandPool(m_device, m_graphicsQueue.family);
    Apply();
}

HeadlessSwapchain::~HeadlessSwapchain()
{
    m_device.WaitIdle();

    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }

    for (auto& swapchainBuffer : m_swapchainBuffers) {
        swapchainBuffer.Destroy(m_device);
    }

    LOGI("Swapchain destroyed");

    m_msaaColorBuffer = nullptr;
    m_msaaDepthBuffer = nullptr;
    m_depthBuffer = nullptr;
}

bool HeadlessSwapchain::UpdateExtent(uint32_t width, uint32_t height)
{
    m_extent = { width, height };
    Apply();
    return true;
}

bool HeadlessSwapchain::SetImageCount(uint32_t imageCount)
{
    m_imageCount = imageCount;
    Apply();
    return true;
}

std::vector<VkPresentModeKHR> HeadlessSwapchain::GetPresentModes() const
{
    return {};
}

bool HeadlessSwapchain::SetPresentMode(bool noTearing, bool powerSave)
{
    return true;
}

bool HeadlessSwapchain::SetPresentMode(VkPresentModeKHR preferredMode)
{
    return true;
}

void HeadlessSwapchain::Print() const
{
    LOGI("Swapchain:");

    LOGI("\tColor   = %3d : %s", m_renderPass.GetColorFormat(), util::vk::FormatToString(m_renderPass.GetColorFormat()).c_str());
    LOGI("\tDepth   = %3d : %s", m_depthBuffer->GetFormat(), util::vk::FormatToString(m_depthBuffer->GetFormat()).c_str());

    LOGI("\tExtent  = %d x %d", m_extent.width, m_extent.height);
    LOGI("\tBuffers = %d", static_cast<int>(m_swapchainBuffers.size()));
}

const VkExtent2D& HeadlessSwapchain::GetExtent() const
{
    return m_extent;
}

uint32_t HeadlessSwapchain::GetImageCount() const
{
    return m_imageCount;
}

void HeadlessSwapchain::Apply()
{
    m_device.WaitIdle();

    for (auto& swapchainBuffer : m_swapchainBuffers) {
        swapchainBuffer.Destroy(m_device);
    }

    const VkExtent3D newExtent{ m_extent.width, m_extent.height, 1 };
    const VkImageViewType imageViewType{ m_viewCount > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D };

    m_depthBuffer = buffer::ImageBufferBuilder{ m_allocator }
                        .SetExtent(newExtent)
                        .SetFormat(m_renderPass.GetDepthFormat())
                        .SetType(VK_IMAGE_TYPE_2D)
                        .SetViewType(imageViewType)
                        .SetLayerCount(m_viewCount)
                        .SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                        .SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                        .Build();
    if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
        m_msaaColorBuffer = buffer::ImageBufferBuilder{ m_allocator }
                                .SetExtent(newExtent)
                                .SetFormat(m_renderPass.GetColorFormat())
                                .SetType(VK_IMAGE_TYPE_2D)
                                .SetViewType(imageViewType)
                                .SetLayerCount(m_viewCount)
                                .SetSampleCount(m_sampleCount)
                                .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                                .SetLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                                .Build();
        m_msaaDepthBuffer = buffer::ImageBufferBuilder{ m_allocator }
                                .SetExtent(newExtent)
                                .SetFormat(m_renderPass.GetDepthFormat())
                                .SetType(VK_IMAGE_TYPE_2D)
                                .SetViewType(imageViewType)
                                .SetLayerCount(m_viewCount)
                                .SetSampleCount(m_sampleCount)
                                .SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                                .SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                                .Build();
    }

    m_frameIndex = util::CircularIndex{ m_imageCount };

    m_swapchainBuffers.resize(m_imageCount);
    for (uint32_t i = 0; i < m_imageCount; ++i) {
        std::shared_ptr<prev::render::buffer::ImageBuffer> colorBuffer = buffer::ImageBufferBuilder{ m_allocator }
                                                                             .SetExtent(newExtent)
                                                                             .SetFormat(m_renderPass.GetColorFormat())
                                                                             .SetType(VK_IMAGE_TYPE_2D)
                                                                             .SetViewType(imageViewType)
                                                                             .SetLayerCount(m_viewCount)
                                                                             .SetSampleCount(VK_SAMPLE_COUNT_1_BIT)
                                                                             .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                                                                             .SetLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                                                                             .Build();
        std::vector<VkImageView> swapchainImageViews;
        if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
            swapchainImageViews.push_back(m_msaaColorBuffer->GetImageView());
            swapchainImageViews.push_back(m_msaaDepthBuffer->GetImageView());
        }
        swapchainImageViews.push_back(colorBuffer->GetImageView()); // Add color buffer (unique)
        swapchainImageViews.push_back(m_depthBuffer->GetImageView()); // Add depth buffer (shared)

        auto& swapchainBuffer{ m_swapchainBuffers[i] };
        swapchainBuffer.colorBuffer = colorBuffer;
        swapchainBuffer.framebuffer = util::vk::CreateFrameBuffer(m_device, m_renderPass, swapchainImageViews, m_extent);
        swapchainBuffer.commandBuffer = util::vk::CreateCommandBuffer(m_device, m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        swapchainBuffer.fence = util::vk::CreateFence(m_device, VK_FENCE_CREATE_SIGNALED_BIT);
    }

    LOGI("Swapchain recreated");
}

bool HeadlessSwapchain::AcquireNext(SwapchainBuffer& next)
{
    ASSERT(!m_isAcquired, "Swapchain: Previous swapchain buffer has not yet been presented.");

    const auto& swapchainBuffer{ m_swapchainBuffers[m_frameIndex] };

    VKERRCHECK(vkWaitForFences(m_device, 1, &swapchainBuffer.fence, VK_TRUE, UINT64_MAX));
    VKERRCHECK(vkResetFences(m_device, 1, &swapchainBuffer.fence));

    m_isAcquired = true;
    next = swapchainBuffer;

    return true;
}

void HeadlessSwapchain::Submit()
{
    ASSERT(!!m_isAcquired, "Swapchain: A buffer must be acquired before submitting.");

    const auto& swapchainBuffer{ m_swapchainBuffers[m_frameIndex] };

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo{ prev::util::vk::CreateStruct<VkSubmitInfo>(VK_STRUCTURE_TYPE_SUBMIT_INFO) };
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &swapchainBuffer.commandBuffer;

    VKERRCHECK(m_graphicsQueue.Submit(1, &submitInfo, swapchainBuffer.fence));

    ++m_frameIndex;
    m_isAcquired = false;
}

bool HeadlessSwapchain::BeginFrame(FrameContext& outContext)
{
    SwapchainBuffer swapchainBuffer;
    if (!AcquireNext(swapchainBuffer)) {
        return false;
    }

    VkCommandBufferBeginInfo beginInfo{ prev::util::vk::CreateStruct<VkCommandBufferBeginInfo>(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO) };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKERRCHECK(vkBeginCommandBuffer(swapchainBuffer.commandBuffer, &beginInfo));

    outContext = { swapchainBuffer.framebuffer, swapchainBuffer.commandBuffer, m_frameIndex };
    return true;
}

void HeadlessSwapchain::EndFrame()
{
    const auto& swapchainBuffer{ m_swapchainBuffers[m_frameIndex] };
    VKERRCHECK(vkEndCommandBuffer(swapchainBuffer.commandBuffer));

    Submit();
}
} // namespace prev::render::swapchain::headless

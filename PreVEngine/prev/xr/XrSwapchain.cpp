#include "XrSwapchain.h"

#ifdef ENABLE_XR

#include "../render/buffer/ImageBufferBuilder.h"
#include "../util/VkUtils.h"

namespace prev::xr {
XrSwapchain::XrSwapchain(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, prev::render::pass::RenderPass& renderPass, xr::OpenXr& xr, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount)
    : m_device{ device }
    , m_allocator{ allocator }
    , m_renderPass{ renderPass }
    , m_openXr{ xr }
    , m_surface{ surface }
    , m_sampleCount{ sampleCount }
    , m_graphicsQueue{ m_device.GetQueue(prev::core::device::QueueType::GRAPHICS) }
    , m_presentQueue{ m_device.GetQueue(prev::core::device::QueueType::PRESENT) }
{
    m_commandPool = prev::util::vk::CreateCommandPool(m_device, m_graphicsQueue.family);

    m_extent = m_openXr.GetExtent();

    if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
        m_msaaColorBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                                .SetExtent(VkExtent3D{ m_extent.width, m_extent.height, 1 })
                                .SetFormat(m_renderPass.GetColorFormat())
                                .SetType(VK_IMAGE_TYPE_2D)
                                .SetViewType(VK_IMAGE_VIEW_TYPE_2D_ARRAY)
                                .SetLayerCount(m_openXr.GetViewCount())
                                .SetSampleCount(m_sampleCount)
                                .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                                .SetLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                                .Build();
        m_msaaDepthBuffer = prev::render::buffer::ImageBufferBuilder{ m_allocator }
                                .SetExtent(VkExtent3D{ m_extent.width, m_extent.height, 1 })
                                .SetFormat(m_renderPass.GetDepthFormat())
                                .SetType(VK_IMAGE_TYPE_2D)
                                .SetViewType(VK_IMAGE_VIEW_TYPE_2D_ARRAY)
                                .SetLayerCount(m_openXr.GetViewCount())
                                .SetSampleCount(m_sampleCount)
                                .SetUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
                                .SetLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                                .Build();
    }

    const auto colorImages{ m_openXr.GetColorImages() };
    const auto colorImageViews{ m_openXr.GetColorImagesViews() };

    const auto depthImages{ m_openXr.GetDepthImages() };
    const auto depthImageViews{ m_openXr.GetDepthImagesViews() };

    const auto swapchainImagesCount{ static_cast<uint32_t>(colorImages.size()) };

    m_swapchainBuffers.resize(swapchainImagesCount);
    for (uint32_t i = 0; i < swapchainImagesCount; ++i) {
        const auto& colorImage{ colorImages[i] };
        const auto& colorImageView{ colorImageViews[i] };
        const auto& depthImage{ depthImages[i] };
        const auto& depthImageView{ depthImageViews[i] };

        std::vector<VkImageView> swapchainImageViews;
        if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
            swapchainImageViews.push_back(m_msaaColorBuffer->GetImageView());
            swapchainImageViews.push_back(m_msaaDepthBuffer->GetImageView());
        }
        swapchainImageViews.push_back(colorImageView);
        swapchainImageViews.push_back(depthImageView);

        auto& swapchainBuffer{ m_swapchainBuffers[i] };
        swapchainBuffer.colorImage = colorImage;
        swapchainBuffer.colorImageView = colorImageView;
        swapchainBuffer.depthImage = depthImage;
        swapchainBuffer.depthImageView = depthImageView;
        swapchainBuffer.framebuffer = util::vk::CreateFrameBuffer(m_device, m_renderPass, swapchainImageViews, m_extent);
        swapchainBuffer.commandBuffer = util::vk::CreateCommandBuffer(m_device, m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        swapchainBuffer.fence = util::vk::CreateFence(m_device, VK_FENCE_CREATE_SIGNALED_BIT);
        swapchainBuffer.extent = m_extent;
    }

    LOGI("XR Swapchain created");
}

XrSwapchain::~XrSwapchain()
{
    m_device.WaitIdle();

    m_msaaDepthBuffer = nullptr;
    m_msaaColorBuffer = nullptr;

    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }

    for (auto& swapchainBuffer : m_swapchainBuffers) {
        swapchainBuffer.Destroy(m_device);
    }

    LOGI("XR Swapchain destroyed\n");
}

bool XrSwapchain::UpdateExtent(uint32_t width, uint32_t height)
{
    return false;
}

bool XrSwapchain::SetImageCount(uint32_t imageCount)
{
    return false;
}

std::vector<VkPresentModeKHR> XrSwapchain::GetPresentModes() const
{
    return {};
}

bool XrSwapchain::SetPresentMode(bool noTearing, bool powerSave)
{
    return false;
}

bool XrSwapchain::SetPresentMode(VkPresentModeKHR preferredMode)
{
    return false;
}

void XrSwapchain::Print() const
{
    LOGI("XR Swapchain:");

    LOGI("\tColor   = %3d : %s", m_renderPass.GetColorFormat(), util::vk::FormatToString(m_renderPass.GetColorFormat()).c_str());
    LOGI("\tDepth   = %3d : %s", m_renderPass.GetDepthFormat(), util::vk::FormatToString(m_renderPass.GetDepthFormat()).c_str());

    LOGI("\tExtent  = %d x %d", m_extent.width, m_extent.height);
    LOGI("\tBuffers = %d", static_cast<int>(m_swapchainBuffers.size()));
}

const VkExtent2D& XrSwapchain::GetExtent() const
{
    return m_extent;
}

uint32_t XrSwapchain::GetImageCount() const
{
    return static_cast<uint32_t>(m_swapchainBuffers.size());
}

bool XrSwapchain::BeginFrame(prev::render::SwapChainFrameContext& outContext)
{
    m_acquiredIndex = m_openXr.GetCurrentSwapchainIndex();

    const auto& swapchainBuffer{ m_swapchainBuffers[m_acquiredIndex] };

    VKERRCHECK(vkWaitForFences(m_device, 1, &swapchainBuffer.fence, VK_TRUE, UINT64_MAX));
    VKERRCHECK(vkResetFences(m_device, 1, &swapchainBuffer.fence));

    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VKERRCHECK(vkBeginCommandBuffer(swapchainBuffer.commandBuffer, &beginInfo));

    outContext = { swapchainBuffer.framebuffer, swapchainBuffer.commandBuffer, m_acquiredIndex };
    return true;
}

void XrSwapchain::EndFrame()
{
    const auto& swapchainBuffer{ m_swapchainBuffers[m_acquiredIndex] };
    VKERRCHECK(vkEndCommandBuffer(swapchainBuffer.commandBuffer));

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &swapchainBuffer.commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    VKERRCHECK(m_graphicsQueue.Submit(1, &submitInfo, swapchainBuffer.fence));
}
} // namespace prev::xr

#endif
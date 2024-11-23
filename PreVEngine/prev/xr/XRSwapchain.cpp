#include "XRSwapchain.h"

#include "../render/buffer/ImageBufferBuilder.h"
#include "../util/VkUtils.h"

namespace prev::xr {
    XRSwapchain::XRSwapchain(core::device::Device& device, core::memory::Allocator& allocator, render::pass::RenderPass& renderPass, xr::OpenXR& xr, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount)
            : m_device{ device }
            , m_allocator{ allocator }
            , m_renderPass{ renderPass }
            , m_openXr{ xr }
            , m_surface{ surface }
            , m_sampleCount{ sampleCount }
    {
        m_graphicsQueue = m_device.GetQueue(core::device::QueueType::GRAPHICS);
        m_presentQueue = m_device.GetQueue(core::device::QueueType::PRESENT);

        m_commandPool = prev::util::vk::CreateCommandPool(m_device, m_graphicsQueue->family);

        m_extent = m_openXr.GetExtent();

        if (m_sampleCount > VK_SAMPLE_COUNT_1_BIT) {
            m_msaaColorBuffer = render::buffer::ImageBufferBuilder{ m_allocator }
                    .SetExtent(VkExtent3D{ m_extent.width, m_extent.height, 1 })
                    .SetFormat(m_renderPass.GetColorFormat())
                    .SetType(VK_IMAGE_TYPE_2D)
                    .SetViewType(VK_IMAGE_VIEW_TYPE_2D_ARRAY)
                    .SetLayerCount(m_openXr.GetViewCount())
                    .SetSampleCount(m_sampleCount)
                    .SetUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                    .SetLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    .Build();
            m_msaaDepthBuffer = render::buffer::ImageBufferBuilder{ m_allocator }
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
    }

    XRSwapchain::~XRSwapchain()
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

        LOGI("Swapchain destroyed\n");
    }

    bool XRSwapchain::UpdateExtent()
    {
        return false;
    }

    bool XRSwapchain::SetImageCount(uint32_t imageCount)
    {
        return false;
    }

    std::vector<VkPresentModeKHR> XRSwapchain::GetPresentModes() const
    {
        return {};
    }

    bool XRSwapchain::SetPresentMode(bool noTearing, bool powerSave)
    {
       return false;
    }

    bool XRSwapchain::SetPresentMode(VkPresentModeKHR preferredMode)
    {
        return false;
    }

    void XRSwapchain::Print() const
    {
        LOGI("XSwapchain:\n");

        LOGI("\tColor   = %3d : %s\n", m_renderPass.GetColorFormat(), util::vk::FormatToString(m_renderPass.GetColorFormat()).c_str());
        LOGI("\tDepth   = %3d : %s\n", m_renderPass.GetDepthFormat(), util::vk::FormatToString(m_renderPass.GetDepthFormat()).c_str());

        LOGI("\tExtent  = %d x %d\n", m_extent.width, m_extent.height);
        LOGI("\tBuffers = %d\n", (int)m_swapchainBuffers.size());
    }

    const VkExtent2D& XRSwapchain::GetExtent() const
    {
        return m_extent;
    }

    uint32_t XRSwapchain::GetImageCount() const
    {
        return static_cast<uint32_t>(m_swapchainBuffers.size());
    }

    bool XRSwapchain::BeginFrame(render::SwapChainFrameContext& outContext)
    {
        m_acquiredIndex = m_openXr.GetCurrentSwapchainIndex();

        const auto& swapchainBuffer{ m_swapchainBuffers[m_acquiredIndex] };

        VKERRCHECK(vkWaitForFences(m_device, 1, &swapchainBuffer.fence, VK_TRUE, UINT64_MAX));
        VKERRCHECK(vkResetFences(m_device, 1, &swapchainBuffer.fence));

        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        VKERRCHECK(vkBeginCommandBuffer(swapchainBuffer.commandBuffer, &beginInfo));

        outContext = { swapchainBuffer.framebuffer, swapchainBuffer.commandBuffer, m_acquiredIndex };
        return true;
    }

    void XRSwapchain::EndFrame()
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

        VKERRCHECK(m_graphicsQueue->Submit(1, &submitInfo, swapchainBuffer.fence));
    }
} // namespace prev::render

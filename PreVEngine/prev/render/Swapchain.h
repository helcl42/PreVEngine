#ifndef __SWAPCHAIN_H__
#define __SWAPCHAIN_H__

#include "ISwapchain.h"

#include "buffer/ImageBuffer.h"
#include "pass/RenderPass.h"

#include "../core/memory/Allocator.h"
#include "../util/Utils.h"

namespace prev::render {
class Swapchain final : public ISwapchain {
public:
    Swapchain(core::device::Device& device, core::memory::Allocator& allocator, pass::RenderPass& renderPass, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT, uint32_t viewCount = 1);

    ~Swapchain();

public:
    std::vector<VkPresentModeKHR> GetPresentModes() const override;

    bool SetPresentMode(bool noTearing, bool powerSave) override;

    bool SetPresentMode(VkPresentModeKHR preferredMode) override;

    bool SetImageCount(uint32_t imageCount) override;

    bool UpdateExtent() override;

    bool BeginFrame(SwapChainFrameContext& outContext) override;

    void EndFrame() override;

    void Print() const;

    const VkExtent2D& GetExtent() const override;

    uint32_t GetImageCount() const override;

private:
    struct SwapchainBuffer {
        VkImage image{};

        VkImageView view{};

        VkExtent2D extent{};

        VkFramebuffer framebuffer{};

        VkCommandBuffer commandBuffer{};

        VkFence fence{};

        VkSemaphore renderSemaphore{};

        VkSemaphore presentSemaphore{};

        void Destroy(VkDevice device)
        {
            vkDestroySemaphore(device, presentSemaphore, nullptr);
            vkDestroySemaphore(device, renderSemaphore, nullptr);
            vkDestroyFence(device, fence, nullptr);
            vkDestroyFramebuffer(device, framebuffer, nullptr);
            vkDestroyImageView(device, view, nullptr);
        }
    };

private:
    void Apply();

    bool AcquireNext(SwapchainBuffer& next);

    void Submit();

    void Present();

    virtual VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const;

    virtual std::vector<VkImage> GetSwapchainImages() const;

private:
    core::device::Device& m_device;

    core::memory::Allocator& m_allocator;

    pass::RenderPass& m_renderPass;

    VkSurfaceKHR m_surface{};

    VkSampleCountFlagBits m_sampleCount{ VK_SAMPLE_COUNT_1_BIT };

    uint32_t m_viewCount{ 1 };

    const prev::core::device::Queue& m_graphicsQueue{};

    const prev::core::device::Queue& m_presentQueue{};

    std::unique_ptr<prev::render::buffer::ImageBuffer> m_depthBuffer{};

    VkSwapchainKHR m_swapchain{};

    VkSwapchainCreateInfoKHR m_swapchainCreateInfo{};

    VkCommandPool m_commandPool{};

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    uint32_t m_acquiredIndex{}; // index of last acquired image

    bool m_isAcquired{};

    util::CircularIndex<uint32_t> m_frameIndex{ 3 };

    // MSAA
    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaColorBuffer{};

    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaDepthBuffer{};
};
} // namespace prev::render

#endif

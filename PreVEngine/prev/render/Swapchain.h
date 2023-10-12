#ifndef __SWAPCHAIN_H__
#define __SWAPCHAIN_H__

#include "buffer/image/IImageBuffer.h"
#include "pass/RenderPass.h"

#include "../core/memory/Allocator.h"

#ifdef ANDROID
#define IS_ANDROID true // ANDROID: default to power-save (limit to 60fps)
#else
#define IS_ANDROID false // PC: default to low-latency (no fps limit)
#endif

namespace prev::render {
struct SwapchainBuffer {
    VkImage image{};

    VkImageView view{};

    VkExtent2D extent{};

    VkFramebuffer framebuffer{};

    VkCommandBuffer commandBuffer{};

    VkFence fence{};

    void Destroy(VkDevice device)
    {
        vkDestroyFence(device, fence, nullptr);
        vkDestroyFramebuffer(device, framebuffer, nullptr);
        vkDestroyImageView(device, view, nullptr);
    }
};

class Swapchain {
public:
    Swapchain(core::device::Device& device, core::memory::Allocator& allocator, pass::RenderPass& renderPass, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

    virtual ~Swapchain();

public:
    std::vector<VkPresentModeKHR> GetPresentModes() const;

    bool SetPresentMode(bool noTearing, bool poweSave = IS_ANDROID); // ANDROID: default to power-save mode (limit to 60fps)

    bool SetPresentMode(VkPresentModeKHR preferredMode);

    bool SetImageCount(uint32_t imageCount = 2);

    bool UpdateExtent();

    bool BeginFrame(VkFramebuffer& frameBuffer, VkCommandBuffer& commandBuffer, uint32_t& acquiredIndex);

    void EndFrame();

    void Print() const;

public:
    const VkExtent2D& GetExtent() const;

    uint32_t GetImageCount() const;

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

    std::shared_ptr<prev::core::device::Queue> m_presentQueue{};

    std::shared_ptr<prev::core::device::Queue> m_graphicsQueue{};

    std::unique_ptr<prev::render::buffer::image::IImageBuffer> m_depthBuffer{};

    VkSwapchainKHR m_swapchain{};

    VkSwapchainCreateInfoKHR m_swapchainCreateInfo{};

    VkCommandPool m_commandPool{};

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    uint32_t m_acquiredIndex{}; // index of last acquired image

    bool m_isAcquired{};

    VkSemaphore m_acquireSemaphore{};

    VkSemaphore m_submitSemaphore{};

    uint32_t m_currentFrameIndex{};

    uint32_t m_swapchainImagesCount{};

    // MSAA
    std::unique_ptr<prev::render::buffer::image::IImageBuffer> m_msaaColorBuffer{};

    std::unique_ptr<prev::render::buffer::image::IImageBuffer> m_msaaDepthBuffer{};
};
} // namespace prev::render

#endif

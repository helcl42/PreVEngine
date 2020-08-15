#ifndef __SWAPCHAIN_H__
#define __SWAPCHAIN_H__

#include "../core/memory/Allocator.h"
#include "../core/memory/image/DepthImageBuffer.h"
#include "pass/RenderPass.h"

#ifdef ANDROID
#define IS_ANDROID true // ANDROID: default to power-save (limit to 60fps)
#else
#define IS_ANDROID false // PC: default to low-latency (no fps limit)
#endif

namespace prev {
struct SwapchainBuffer {
    VkImage image;

    VkImageView view; // TODO: MRT?

    VkExtent2D extent;

    VkFramebuffer framebuffer;

    VkCommandBuffer commandBuffer;

    VkFence fence;
};

class Swapchain {
public:
    Swapchain(const Queue& presentQueue, const Queue& graphicsQueue, RenderPass& renderPass, Allocator& allocator);

    virtual ~Swapchain();

public:
    bool SetPresentMode(bool noTearing, bool poweSave = IS_ANDROID); // ANDROID: default to power-save mode (limit to 60fps)

    bool SetPresentMode(VkPresentModeKHR preferredMode);

    bool SetImageCount(uint32_t imageCount = 2);

    void UpdateExtent();

    bool BeginFrame(VkFramebuffer& frmmeBuffer, VkCommandBuffer& buffer, uint32_t& acquiredIndex);

    void EndFrame();

    void Print() const;

    const Queue& GetPresentQueue() const;

    const Queue& GetGraphicsQueue() const;

public:
    const VkExtent2D& GetExtent() const;

    uint32_t GetmageCount() const;

private:
    void Init();

    void Apply();

    bool AcquireNext(SwapchainBuffer& next);

    void Submit();

    void Present();

    virtual VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const;

    virtual std::vector<VkImage> GetSwapchainImages() const;

private:
    const Queue& m_presentQueue;

    const Queue& m_graphicsQueue;

    RenderPass& m_renderPass;

    Allocator& m_allocator;

    VkPhysicalDevice m_gpu;

    VkDevice m_device;

    VkSurfaceKHR m_surface;

    DepthImageBuffer m_depthBuffer;

    VkSwapchainKHR m_swapchain;

    VkSwapchainCreateInfoKHR m_swapchainCreateInfo;

    VkCommandPool m_commandPool;

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    uint32_t m_acquiredIndex; // index of last acquired image

    bool m_isAcquired;

    VkSemaphore m_acquireSemaphore;

    VkSemaphore m_submitSemaphore;

    uint32_t m_currentFrameIndex;

    uint32_t m_swapchainImagesCount;
};
} // namespace prev

#endif

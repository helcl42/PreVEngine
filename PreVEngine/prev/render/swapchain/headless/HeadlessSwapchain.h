#ifndef __HEADLESS_SWAPCHAIN_H__
#define __HEADLESS_SWAPCHAIN_H__

#include "../ISwapchain.h"

#include "../../buffer/ImageBuffer.h"
#include "../../pass/RenderPass.h"

#include "../../../core/memory/Allocator.h"
#include "../../../util/Utils.h"

namespace prev::render::swapchain::headless {
class HeadlessSwapchain final : public ISwapchain {
public:
    HeadlessSwapchain(core::device::Device& device, core::memory::Allocator& allocator, pass::RenderPass& renderPass, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT, uint32_t viewCount = 1);

    ~HeadlessSwapchain();

public:
    std::vector<VkPresentModeKHR> GetPresentModes() const override;

    bool SetPresentMode(bool noTearing, bool powerSave) override;

    bool SetPresentMode(VkPresentModeKHR preferredMode) override;

    bool SetImageCount(uint32_t imageCount) override;

    bool UpdateExtent(uint32_t width, uint32_t height) override;

    bool BeginFrame(FrameContext& outContext) override;

    void EndFrame() override;

    void Print() const override;

    const VkExtent2D& GetExtent() const override;

    uint32_t GetImageCount() const override;

private:
    struct SwapchainBuffer {
        std::shared_ptr<prev::render::buffer::ImageBuffer> colorBuffer{};

        VkFramebuffer framebuffer{};

        VkCommandBuffer commandBuffer{};

        VkFence fence{};

        void Destroy(VkDevice device)
        {
            vkDestroyFence(device, fence, nullptr);
            vkDestroyFramebuffer(device, framebuffer, nullptr);
            colorBuffer = nullptr;
        }
    };

private:
    void Apply();

    bool AcquireNext(SwapchainBuffer& next);

    void Submit();

private:
    core::device::Device& m_device;

    core::memory::Allocator& m_allocator;

    pass::RenderPass& m_renderPass;

    VkSampleCountFlagBits m_sampleCount{ VK_SAMPLE_COUNT_1_BIT };

    uint32_t m_viewCount{ 1 };

    const prev::core::device::Queue& m_graphicsQueue;

    VkExtent2D m_extent{};

    uint32_t m_imageCount{ 3 };

    std::unique_ptr<prev::render::buffer::ImageBuffer> m_depthBuffer{};

    VkCommandPool m_commandPool{};

    std::vector<SwapchainBuffer> m_swapchainBuffers;

    bool m_isAcquired{};

    util::CircularIndex<uint32_t> m_frameIndex{ 3 };

    // MSAA
    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaColorBuffer{};

    std::unique_ptr<prev::render::buffer::ImageBuffer> m_msaaDepthBuffer{};
};
} // namespace prev::render::swapchain::headless

#endif

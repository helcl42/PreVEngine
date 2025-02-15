#ifndef __ISWAPCHAIN_H__
#define __ISWAPCHAIN_H__

#include "../core/Core.h"

#include <vector>

namespace prev::render {
struct SwapChainFrameContext {
    VkFramebuffer frameBuffer{};
    VkCommandBuffer commandBuffer{};
    uint32_t index{};
};

class ISwapchain {
public:
    virtual std::vector<VkPresentModeKHR> GetPresentModes() const = 0;

    virtual bool SetPresentMode(bool noTearing, bool powerSave) = 0;

    virtual bool SetPresentMode(VkPresentModeKHR preferredMode) = 0;

    virtual bool SetImageCount(uint32_t imageCount) = 0;

    virtual bool UpdateExtent(uint32_t width, uint32_t height) = 0;

    virtual bool BeginFrame(SwapChainFrameContext& outContext) = 0;

    virtual void EndFrame() = 0;

    virtual void Print() const = 0;

    virtual const VkExtent2D& GetExtent() const = 0;

    virtual uint32_t GetImageCount() const = 0;

public:
    virtual ~ISwapchain() = default;
};
} // namespace prev::render

#endif
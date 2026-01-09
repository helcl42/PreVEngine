#include "SwapchainFactory.h"

#include "headless/HeadlessSwapchain.h"
#include "presentable/PresentableSwapchain.h"

namespace prev::render::swapchain {
std::unique_ptr<ISwapchain> SwapchainFactory::Create(core::device::Device& device, core::memory::Allocator& allocator, pass::RenderPass& renderPass, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount, uint32_t viewCount, uint32_t maxFramesInFlight) const
{
    if (!surface) {
        LOGW("Creating headless swapchain");
        return std::make_unique<headless::HeadlessSwapchain>(device, allocator, renderPass, sampleCount, viewCount);
    }
    return std::make_unique<presentable::PresentableSwapchain>(device, allocator, renderPass, surface, sampleCount, viewCount, maxFramesInFlight);
}
} // namespace prev::render::swapchain
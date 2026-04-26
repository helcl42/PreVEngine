#include "SwapchainFactory.h"

#include "headless/HeadlessSwapchain.h"
#include "presentable/PresentableSwapchain.h"

#include "../../common/Logger.h"

namespace prev::render::swapchain {
std::unique_ptr<ISwapchain> SwapchainFactory::Create(core::device::Device& device, pass::RenderPass& renderPass, GfxSurface surface, GfxExtent2D extent, GfxPresentMode presentMode, uint32_t imageCount, uint32_t viewCount, uint32_t maxFramesInFlight) const
{
    if (!surface) {
        LOGW("Creating headless swapchain");
        return std::make_unique<headless::HeadlessSwapchain>(device, renderPass, extent, imageCount, viewCount);
    }
    return std::make_unique<presentable::PresentableSwapchain>(device, renderPass, surface, extent, presentMode, imageCount, viewCount, maxFramesInFlight);
}
} // namespace prev::render::swapchain

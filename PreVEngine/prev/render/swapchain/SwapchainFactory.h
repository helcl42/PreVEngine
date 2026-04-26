#ifndef __SWAPCHAIN_FACTORY_H__
#define __SWAPCHAIN_FACTORY_H__

#include "ISwapchain.h"

#include "../pass/RenderPass.h"

#include "../../core/device/Device.h"

#include <memory>

namespace prev::render::swapchain {
class SwapchainFactory final {
public:
    std::unique_ptr<ISwapchain> Create(core::device::Device& device, pass::RenderPass& renderPass, GfxSurface surface, GfxExtent2D extent, GfxPresentMode presentMode, uint32_t imageCount, uint32_t viewCount = 1, uint32_t maxFramesInFlight = 0) const;
};
} // namespace prev::render::swapchain

#endif

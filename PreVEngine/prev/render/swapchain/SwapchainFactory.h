#ifndef __SWAPCHAIN_FACTORY_H__
#define __SWAPCHAIN_FACTORY_H__

#include "ISwapchain.h"

#include "../pass/RenderPass.h"

#include "../../core/device/Device.h"
#include "../../core/memory/Allocator.h"

#include <memory>

namespace prev::render::swapchain {
class SwapchainFactory final {
public:
    std::unique_ptr<ISwapchain> Create(core::device::Device& device, core::memory::Allocator& allocator, pass::RenderPass& renderPass, VkSurfaceKHR surface, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT, uint32_t viewCount = 1) const;
};
} // namespace prev::render::swapchain

#endif
#ifndef __SWAPCHAIN_TARGETS_H__
#define __SWAPCHAIN_TARGETS_H__

#include "../buffer/ImageBuffer.h"
#include "../framebuffer/Framebuffer.h"
#include "../pass/RenderPass.h"

#include "../../core/device/Device.h"

#include <memory>

namespace prev::render::swapchain {

std::unique_ptr<buffer::ImageBuffer> CreateRenderAttachment(core::device::Device& device, GfxExtent2D extent, GfxFormat format, GfxSampleCount sampleCount, uint32_t viewCount);

class SwapchainTargets final {
public:
    SwapchainTargets(core::device::Device& device, pass::RenderPass& renderPass, GfxExtent2D extent, GfxFormat colorFormat, GfxFormat depthFormat, GfxSampleCount sampleCount, uint32_t viewCount, bool createSharedDepth);

    std::unique_ptr<framebuffer::Framebuffer> CreateFramebuffer(GfxTextureView colorView, GfxTextureView depthView = nullptr) const;

private:
    core::device::Device& m_device;

    pass::RenderPass& m_renderPass;

    GfxExtent2D m_extent{};

    GfxSampleCount m_sampleCount{ GFX_SAMPLE_COUNT_1 };

    std::unique_ptr<buffer::ImageBuffer> m_depth; // shared depth (when createSharedDepth)

    std::unique_ptr<buffer::ImageBuffer> m_msaaColor; // when sampleCount > 1

    std::unique_ptr<buffer::ImageBuffer> m_msaaDepth; // when sampleCount > 1
};

} // namespace prev::render::swapchain

#endif // !__SWAPCHAIN_TARGETS_H__

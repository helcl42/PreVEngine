#ifndef __OFF_SCREEN_RENDER_PASS_COMPONENT_H__
#define __OFF_SCREEN_RENDER_PASS_COMPONENT_H__

#include "IOffScreenRenderPassComponent.h"

#include <prev/common/Common.h>
#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/render/buffer/ImageBuffer.h>

#include <vector>

namespace prev_test::component::common {
class OffScreenRenderPassComponent final : public IOffScreenRenderPassComponent {
public:
    OffScreenRenderPassComponent(prev::core::device::Device& device, const VkExtent2D& extent, const std::shared_ptr<prev::render::pass::RenderPass>& renderPass, const std::shared_ptr<prev::render::buffer::ImageBuffer>& depthBuffer, const std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>>& colorBuffers, const VkFramebuffer frameBuffer);

    ~OffScreenRenderPassComponent();

public:
    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const override;

    const VkExtent2D& GetExtent() const override;

    std::shared_ptr<prev::render::buffer::ImageBuffer> GetColorImageBuffer(const uint32_t index = 0) const override;

    std::shared_ptr<prev::render::buffer::ImageBuffer> GetDepthImageBuffer() const override;

    VkFramebuffer GetFrameBuffer() const override;

private:
    prev::core::device::Device& m_device;

    VkExtent2D m_extent{};

    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass{};

    std::shared_ptr<prev::render::buffer::ImageBuffer> m_depthBuffer{};

    std::vector<std::shared_ptr<prev::render::buffer::ImageBuffer>> m_colorBuffers{};

    VkFramebuffer m_frameBuffer{};
};

} // namespace prev_test::component::common

#endif
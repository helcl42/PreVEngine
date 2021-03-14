#ifndef __OFF_SCREEN_RENDER_PASS_COMPONENT_H__
#define __OFF_SCREEN_RENDER_PASS_COMPONENT_H__

#include "IOffScreenRenderPassComponent.h"

#include <prev/common/Common.h>
#include <prev/core/memory/image/ColorImageBuffer.h>
#include <prev/core/memory/image/DepthImageBuffer.h>

namespace prev_test::component::common {
class OffScreenRenderPassComponent : public IOffScreenRenderPassComponent {
public:
    static const inline VkFormat COLOR_FORMAT{ VK_FORMAT_B8G8R8A8_UNORM };

    static const inline VkFormat DEPTH_FORMAT{ VK_FORMAT_D32_SFLOAT };

public:
    OffScreenRenderPassComponent(const uint32_t w, const uint32_t h);

    ~OffScreenRenderPassComponent() = default;

public:
    void Init() override;

    void ShutDown() override;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const override;

    VkExtent2D GetExtent() const override;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetColorImageBuffer() const override;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> GetDepthImageBuffer() const override;

    VkFramebuffer GetFrameBuffer() const override;

private:
    void InitBuffers();

    void ShutDownBuffers();

    void InitRenderPass();

    void ShutDownRenderPass();

private:
    const uint32_t m_width;

    const uint32_t m_height;

    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_imageBuffer;

    std::shared_ptr<prev::core::memory::image::IImageBuffer> m_depthBuffer;

    VkFramebuffer m_frameBuffer;
};
} // namespace prev_test::component::common

#endif
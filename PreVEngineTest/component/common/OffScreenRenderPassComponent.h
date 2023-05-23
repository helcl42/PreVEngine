#ifndef __OFF_SCREEN_RENDER_PASS_COMPONENT_H__
#define __OFF_SCREEN_RENDER_PASS_COMPONENT_H__

#include "IOffScreenRenderPassComponent.h"

#include <prev/common/Common.h>
#include <prev/render/buffer/image/ColorImageBuffer.h>
#include <prev/render/buffer/image/DepthImageBuffer.h>

#include <vector>

namespace prev_test::component::common {
class OffScreenRenderPassComponent : public IOffScreenRenderPassComponent {
public:
    OffScreenRenderPassComponent(const VkExtent2D& extent, const VkFormat depthFormat, const std::vector<VkFormat>& colorFormats);

    ~OffScreenRenderPassComponent() = default;

public:
    void Init() override;

    void ShutDown() override;

    std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const override;

    const VkExtent2D& GetExtent() const override;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> GetColorImageBuffer(const uint32_t index = 0) const override;

    std::shared_ptr<prev::render::sampler::Sampler> GetColorSampler(const uint32_t index = 0) const override;

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> GetDepthImageBuffer() const override;

    std::shared_ptr<prev::render::sampler::Sampler> GetDepthSampler() const override;

    VkFramebuffer GetFrameBuffer() const override;

private:
    VkExtent2D m_extent{};

    VkFormat m_depthFormat{ VK_FORMAT_UNDEFINED };

    std::vector<VkFormat> m_colorFormats{};

    std::shared_ptr<prev::render::pass::RenderPass> m_renderPass{};

    std::shared_ptr<prev::render::buffer::image::IImageBuffer> m_depthBuffer{};

    std::shared_ptr<prev::render::sampler::Sampler> m_depthSampler{};

    std::vector<std::shared_ptr<prev::render::buffer::image::IImageBuffer>> m_colorBuffers{};

    std::vector<std::shared_ptr<prev::render::sampler::Sampler>> m_colorSamplers{};

    VkFramebuffer m_frameBuffer{};
};

} // namespace prev_test::component::common

#endif
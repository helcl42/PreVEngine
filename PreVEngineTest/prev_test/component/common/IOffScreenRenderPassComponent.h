#ifndef __IOFF_SCREEN_RENDER_PASS_COMPONENT_H__
#define __IOFF_SCREEN_RENDER_PASS_COMPONENT_H__

#include <prev/render/buffer/ImageBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/sampler/Sampler.h>

#include <memory>

namespace prev_test::component::common {
class IOffScreenRenderPassComponent {
public:
    virtual std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const = 0;

    virtual const VkExtent2D& GetExtent() const = 0;

    virtual std::shared_ptr<prev::render::buffer::ImageBuffer> GetColorImageBuffer(const uint32_t index = 0) const = 0;

    virtual std::shared_ptr<prev::render::sampler::Sampler> GetColorSampler(const uint32_t index = 0) const = 0;

    virtual std::shared_ptr<prev::render::buffer::ImageBuffer> GetDepthImageBuffer() const = 0;

    virtual std::shared_ptr<prev::render::sampler::Sampler> GetDepthSampler() const = 0;

    virtual VkFramebuffer GetFrameBuffer() const = 0;

public:
    virtual ~IOffScreenRenderPassComponent() = default;
};
} // namespace prev_test::component::common

#endif // !__IOFFSCREEN_RENDER_PASS_COMPONENT_H__

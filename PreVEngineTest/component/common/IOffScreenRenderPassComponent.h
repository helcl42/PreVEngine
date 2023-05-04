#ifndef __IOFF_SCREEN_RENDER_PASS_COMPONENT_H__
#define __IOFF_SCREEN_RENDER_PASS_COMPONENT_H__

#include <prev/core/memory/image/IImageBuffer.h>
#include <prev/render/pass/RenderPass.h>
#include <prev/render/sampler/Sampler.h>

namespace prev_test::component::common {
class IOffScreenRenderPassComponent {
public:
    virtual void Init() = 0;

    virtual void ShutDown() = 0;

    virtual std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::IImageBuffer> GetColorImageBuffer() const = 0;

    virtual std::shared_ptr<prev::render::sampler::Sampler> GetColorSampler() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::IImageBuffer> GetDepthImageBuffer() const = 0;

    virtual std::shared_ptr<prev::render::sampler::Sampler> GetDepthSampler() const = 0;

    virtual VkFramebuffer GetFrameBuffer() const = 0;

public:
    virtual ~IOffScreenRenderPassComponent() = default;
};
} // namespace prev_test::component::common

#endif // !__IOFFSCREEN_RENDER_PASS_COMPONENT_H__

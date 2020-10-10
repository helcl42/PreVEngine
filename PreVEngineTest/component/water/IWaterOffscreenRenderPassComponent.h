#ifndef __IWATER_OFFSCREEN_RENDER_PASS_COMPONENT_H__
#define __IWATER_OFFSCREEN_RENDER_PASS_COMPONENT_H__

#include <prev/core/memory/image/IImageBuffer.h>
#include <prev/render/pass/RenderPass.h>

namespace prev_test::component::water {
class IWaterOffscreenRenderPassComponent {
public:
    virtual void Init() = 0;

    virtual void ShutDown() = 0;

    virtual std::shared_ptr<prev::render::pass::RenderPass> GetRenderPass() const = 0;

    virtual VkExtent2D GetExtent() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::IImageBuffer> GetColorImageBuffer() const = 0;

    virtual std::shared_ptr<prev::core::memory::image::IImageBuffer> GetDepthImageBuffer() const = 0;

    virtual VkFramebuffer GetFrameBuffer() const = 0;

public:
    virtual ~IWaterOffscreenRenderPassComponent() = default;
};
} // namespace prev_test::component::water

#endif // !__IWATER_OFFSCREEN_RENDER_PASS_COMPONENT_H__

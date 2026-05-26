#ifndef __FRAMEBUFFER_BUILDER_H__
#define __FRAMEBUFFER_BUILDER_H__

#include "Framebuffer.h"

#include "../pass/RenderPass.h"

#include <memory>
#include <vector>

namespace prev::render::framebuffer {

class FramebufferBuilder final {
public:
    FramebufferBuilder(const prev::core::device::Device& device, const prev::render::pass::RenderPass& renderPass);

    ~FramebufferBuilder() = default;

public:
    FramebufferBuilder& SetExtent(GfxExtent2D extent);

    FramebufferBuilder& AddColorAttachment(GfxTextureView view, GfxTextureView resolveTarget = nullptr);

    FramebufferBuilder& SetDepthStencilAttachment(GfxTextureView view, GfxTextureView resolveTarget = nullptr);

    std::unique_ptr<Framebuffer> Build() const;

private:
    const prev::core::device::Device& m_device;

    const prev::render::pass::RenderPass& m_renderPass;

    GfxExtent2D m_extent{};

    std::vector<GfxFramebufferAttachment> m_colorAttachments;

    GfxFramebufferAttachment m_depthStencilAttachment{};
};

} // namespace prev::render::framebuffer

#endif

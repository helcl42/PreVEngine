#include "FramebufferBuilder.h"

#include <stdexcept>

namespace prev::render::framebuffer {
FramebufferBuilder::FramebufferBuilder(const prev::core::device::Device& device, const prev::render::pass::RenderPass& renderPass)
    : m_device{ device }
    , m_renderPass{ renderPass }
{
}

FramebufferBuilder& FramebufferBuilder::SetExtent(GfxExtent2D extent)
{
    m_extent = extent;
    return *this;
}

FramebufferBuilder& FramebufferBuilder::AddColorAttachment(GfxTextureView view, GfxTextureView resolveTarget)
{
    GfxFramebufferAttachment attachment{};
    attachment.view = view;
    attachment.resolveTarget = resolveTarget;
    m_colorAttachments.push_back(attachment);
    return *this;
}

FramebufferBuilder& FramebufferBuilder::SetDepthStencilAttachment(GfxTextureView view, GfxTextureView resolveTarget)
{
    m_depthStencilAttachment.view = view;
    m_depthStencilAttachment.resolveTarget = resolveTarget;
    return *this;
}

std::unique_ptr<Framebuffer> FramebufferBuilder::Build() const
{
    GfxFramebufferDescriptor fbDesc{};
    fbDesc.sType = GFX_STRUCTURE_TYPE_FRAMEBUFFER_DESCRIPTOR;
    fbDesc.renderPass = m_renderPass;
    fbDesc.colorAttachments = m_colorAttachments.empty() ? nullptr : m_colorAttachments.data();
    fbDesc.colorAttachmentCount = static_cast<uint32_t>(m_colorAttachments.size());
    fbDesc.depthStencilAttachment = m_depthStencilAttachment;
    fbDesc.extent = m_extent;

    auto framebuffer = std::unique_ptr<Framebuffer>(new Framebuffer(m_device));
    if (gfxDeviceCreateFramebuffer(m_device, &fbDesc, &framebuffer->m_framebuffer) != GFX_RESULT_SUCCESS || !framebuffer->m_framebuffer) {
        throw std::runtime_error("Failed to create framebuffer");
    }
    return framebuffer;
}
} // namespace prev::render::framebuffer

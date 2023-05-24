#include "SubPass.h"
#include "RenderPass.h"

namespace prev::render::pass {
namespace {
    VkAttachmentReference CreateAttachmentReference(const RenderPass& renderpass, const uint32_t attachmentIndex)
    {
        VkAttachmentReference reference = {};
        reference.attachment = attachmentIndex;
        if (renderpass.GetAttachments().at(attachmentIndex).finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
            reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        } else {
            reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
        return reference;
    }
} // namespace

SubPass::SubPass(const RenderPass& renderpass)
    : m_renderPass(renderpass)
{
}

SubPass::operator VkSubpassDescription()
{
    VkSubpassDescription subPass = {};
    subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subPass.inputAttachmentCount = static_cast<uint32_t>(m_inputReferences.size());
    subPass.pInputAttachments = m_inputReferences.data();
    subPass.colorAttachmentCount = static_cast<uint32_t>(m_colorReferences.size());
    subPass.pColorAttachments = m_colorReferences.data();
    subPass.pResolveAttachments = m_resolveReferences.data();
    subPass.pDepthStencilAttachment = m_depthReference.attachment != UINT32_MAX ? &m_depthReference : nullptr;
    subPass.preserveAttachmentCount = 0;
    subPass.pPreserveAttachments = nullptr;
    return subPass;
}

void SubPass::UseAttachment(const uint32_t attachmentIndex)
{
    const auto ref{ CreateAttachmentReference(m_renderPass, attachmentIndex) };
    if (ref.layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) { // depth-stencil attachment
        m_depthReference = ref;
    } else { // color attachment
        m_colorReferences.push_back(ref);
    }
}

void SubPass::UseAttachments(const std::vector<uint32_t>& attachmentIndices)
{
    for (const auto& i : attachmentIndices) {
        UseAttachment(i);
    }
}

void SubPass::UseResolveAttachment(const uint32_t attachmentIndex)
{
    const auto ref{ CreateAttachmentReference(m_renderPass, attachmentIndex) };
    m_resolveReferences.push_back(ref);
}

void SubPass::UseResolveAttachments(const std::vector<uint32_t>& attachmentIndices)
{
    for (const auto& i : attachmentIndices) {
        UseResolveAttachment(i);
    }
}

void SubPass::InputAttachment(const uint32_t attachmentIndex)
{
    const auto ref{ CreateAttachmentReference(m_renderPass, attachmentIndex) };
    m_inputReferences.push_back(ref);
}

void SubPass::InputAttachments(const std::vector<uint32_t>& attachmentIndices)
{
    for (const auto& i : attachmentIndices) {
        InputAttachment(i);
    }
}
} // namespace prev::render::pass
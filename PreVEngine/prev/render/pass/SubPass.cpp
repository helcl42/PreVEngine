#include "SubPass.h"
#include "RenderPass.h"

namespace prev::render::pass {
SubPass::SubPass(RenderPass& renderpass)
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

void SubPass::UseAttachment(uint32_t attachmentIndex)
{
    if (m_renderPass.GetAttachments().at(attachmentIndex).finalLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL) { // depth-stencil attachment
        m_depthReference.attachment = attachmentIndex;
        m_depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    } else { // color attachment
        VkAttachmentReference ref = {};
        ref.attachment = attachmentIndex;
        ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        m_colorReferences.push_back(ref);
    }
}

void SubPass::UseAttachments(const std::vector<uint32_t>& attachmentIndexes)
{
    for (const auto& i : attachmentIndexes) {
        UseAttachment(i);
    }
}

void SubPass::UseResolveAttachment(uint32_t attachmentIndex)
{
    VkAttachmentReference ref = {};
    ref.attachment = attachmentIndex;
    ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    m_resolveReferences.push_back(ref);
}

void SubPass::UseResolveAttachments(const std::vector<uint32_t>& attachmentIndexes)
{
    for (const auto& i : attachmentIndexes) {
        UseResolveAttachment(i);
    }
}

void SubPass::InputAttachment(uint32_t attachmentIndex)
{
    VkAttachmentReference reference = {};
    reference.attachment = attachmentIndex;
    reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    m_inputReferences.push_back(reference);
}

void SubPass::InputAttachments(const std::vector<uint32_t>& attachmentIndexes)
{
    for (auto i : attachmentIndexes) {
        InputAttachment(i);
    }
}
} // namespace prev::render::pass
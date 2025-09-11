#include "SubPass.h"
#include "RenderPass.h"

#include "../../core/Formats.h"

namespace prev::render::pass {
namespace {
    VkAttachmentReference CreateAttachmentReference(const uint32_t attachmentIndex, const VkFormat format)
    {
        VkAttachmentReference reference{};
        reference.attachment = attachmentIndex;
        if (prev::core::format::HasDepthComponent(format)) {
            reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        } else {
            reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
        return reference;
    }
} // namespace

SubPass::SubPass(const std::vector<VkAttachmentDescription>& attachments)
    : m_attachments{ attachments }
{
}

SubPass::operator VkSubpassDescription()
{
    VkSubpassDescription subPass{};
    subPass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subPass.inputAttachmentCount = static_cast<uint32_t>(m_inputReferences.size());
    subPass.pInputAttachments = m_inputReferences.data();
    subPass.colorAttachmentCount = static_cast<uint32_t>(m_colorReferences.size());
    subPass.pColorAttachments = m_colorReferences.data();
    subPass.pResolveAttachments = m_resolveReferences.data();
    subPass.pDepthStencilAttachment = m_depthReference.layout != VK_IMAGE_LAYOUT_UNDEFINED ? &m_depthReference : nullptr;
    subPass.preserveAttachmentCount = 0;
    subPass.pPreserveAttachments = nullptr;
    return subPass;
}

void SubPass::UseAttachment(const uint32_t attachmentIndex)
{
    const auto& attachment{ m_attachments.at(attachmentIndex) };
    const auto ref{ CreateAttachmentReference(attachmentIndex, attachment.format) };
    if (prev::core::format::HasDepthComponent(attachment.format)) {
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
    const auto& attachment{ m_attachments.at(attachmentIndex) };
    const auto ref{ CreateAttachmentReference(attachmentIndex, attachment.format) };
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
    const auto& attachment{ m_attachments.at(attachmentIndex) };
    const auto ref{ CreateAttachmentReference(attachmentIndex, attachment.format) };
    m_inputReferences.push_back(ref);
}

void SubPass::InputAttachments(const std::vector<uint32_t>& attachmentIndices)
{
    for (const auto& i : attachmentIndices) {
        InputAttachment(i);
    }
}
} // namespace prev::render::pass
#include "RenderPassBuilder.h"

#include "../../common/Logger.h"
#include "../../core/Formats.h"

namespace prev::render::pass {
RenderPassBuilder::RenderPassBuilder(VkDevice device)
    : m_device(device)
{
}

RenderPassBuilder& RenderPassBuilder::AddColorAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkClearColorValue clearVal, const VkImageLayout finalLayout, const VkAttachmentLoadOp loadOp, const VkAttachmentStoreOp storeOp, const bool resolveAttachment)
{
    VkClearValue clearColor{};
    clearColor.color = clearVal;

    m_attachmentInfos.push_back(AttachmentCreateInfo{ clearColor, format, sampleCount, finalLayout, loadOp, storeOp, resolveAttachment });

    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddDepthAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkClearDepthStencilValue clearVal, const VkImageLayout finalLayout, const VkAttachmentLoadOp loadOp, const VkAttachmentStoreOp storeOp, const bool resolveAttachment)
{
    VkClearValue clearDepth{};
    clearDepth.depthStencil = clearVal;

    m_attachmentInfos.push_back(AttachmentCreateInfo{ clearDepth, format, sampleCount, finalLayout, loadOp, storeOp, resolveAttachment });

    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddSubpass(const std::vector<uint32_t>& attachmentIndices, const std::vector<uint32_t>& resolveIndices)
{
    m_subPassCreateInfos.push_back({ attachmentIndices, resolveIndices });

    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddSubpassDependencies(const std::vector<VkSubpassDependency>& dependencies)
{
    m_dependencies.insert(m_dependencies.end(), dependencies.cbegin(), dependencies.cend());

    return *this;
}

std::unique_ptr<RenderPass> RenderPassBuilder::Build() const
{
    auto renderPass{ std::make_unique<RenderPass>(m_device) };
    renderPass->m_dependencies = m_dependencies;

    for (const auto& attachmentCreateInfo : m_attachmentInfos) {
        renderPass->m_clearValues.push_back(attachmentCreateInfo.clearValue);
        if (prev::core::format::HasDepthComponent(attachmentCreateInfo.format)) {
            renderPass->m_depthFormats.push_back(attachmentCreateInfo.format);
            renderPass->m_depthResolveAttachments.push_back(attachmentCreateInfo.resolveAttachment);
        } else {
            renderPass->m_colorFormats.push_back(attachmentCreateInfo.format);
            renderPass->m_colorResolveAttachments.push_back(attachmentCreateInfo.resolveAttachment);
        }
        renderPass->m_attachments.push_back(CreateAttachmentDescription(attachmentCreateInfo.format, attachmentCreateInfo.sampleCount, attachmentCreateInfo.finalLayout, attachmentCreateInfo.loadOp, attachmentCreateInfo.storeOp));
    }

    for (const auto& subPassCreateInfo : m_subPassCreateInfos) {
        renderPass->m_subpasses.push_back(SubPass(*renderPass));
        auto& subpass{ renderPass->m_subpasses.back() };
        subpass.UseAttachments(subPassCreateInfo.attachmentIndices);
        subpass.UseResolveAttachments(subPassCreateInfo.resolveIndices);
    }

    std::vector<VkSubpassDescription> subpassDescriptions;
    subpassDescriptions.insert(subpassDescriptions.end(), renderPass->m_subpasses.begin(), renderPass->m_subpasses.end());

    VkRenderPassCreateInfo renderPassCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(renderPass->m_attachments.size());
    renderPassCreateInfo.pAttachments = renderPass->m_attachments.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(renderPass->m_dependencies.size());
    renderPassCreateInfo.pDependencies = renderPass->m_dependencies.data();
    VKERRCHECK(vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &renderPass->m_renderPass));

    LOGI("Renderpass created\n");

    return renderPass;
}

VkAttachmentDescription RenderPassBuilder::CreateAttachmentDescription(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkImageLayout finalLayout, const VkAttachmentLoadOp loadOp, const VkAttachmentStoreOp storeOp)
{
    VkAttachmentDescription attachment = {};
    attachment.format = format;
    attachment.samples = sampleCount;
    attachment.loadOp = loadOp;
    attachment.storeOp = storeOp;
    attachment.stencilLoadOp = loadOp;
    attachment.stencilStoreOp = storeOp;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = finalLayout;
    return attachment;
}
} // namespace prev::render::pass
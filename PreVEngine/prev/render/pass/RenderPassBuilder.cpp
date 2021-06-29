#include "RenderPassBuilder.h"

namespace prev::render::pass {
RenderPassBuilder::RenderPassBuilder(VkDevice device)
    : m_device(device)
{
}

RenderPassBuilder& RenderPassBuilder::AddColorAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkClearColorValue clearVal, const VkImageLayout finalLayout)
{
    VkClearValue clearColor{};
    clearColor.color = clearVal;

    m_attachmentInfos.push_back(AttachmentCreateInfo{ clearColor, format, sampleCount, finalLayout });

    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddDepthAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkClearDepthStencilValue clearVal)
{
    VkClearValue clearDepth{};
    clearDepth.depthStencil = clearVal;

    m_attachmentInfos.push_back(AttachmentCreateInfo{ clearDepth, format, sampleCount, DEFAULT_DEPTH_LAYOUT });

    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddSubpass(const std::vector<uint32_t>& attachmentIndexes, const std::vector<uint32_t>& resolveIndices)
{
    m_subPassCreateInfos.push_back({ attachmentIndexes, resolveIndices });

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

    for (const auto& attchmentCreateInfo : m_attachmentInfos) {
        renderPass->m_clearValues.push_back(attchmentCreateInfo.clearValue);
        if (attchmentCreateInfo.finalLayout == DEFAULT_DEPTH_LAYOUT) {
            renderPass->m_depthFormats.push_back(attchmentCreateInfo.format);
        } else {
            renderPass->m_colorFormats.push_back(attchmentCreateInfo.format);
        }
        renderPass->m_attachments.push_back(CreateAttachmentDescription(attchmentCreateInfo.format, attchmentCreateInfo.sampleCount, attchmentCreateInfo.finalLayout));
    }

    for (const auto& subPassCreateInfo : m_subPassCreateInfos) {
        renderPass->m_subpasses.push_back(SubPass(*renderPass));
        auto& subpass{ renderPass->m_subpasses.back() };
        subpass.UseAttachments(subPassCreateInfo.attachmentIndexes);
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

VkAttachmentDescription RenderPassBuilder::CreateAttachmentDescription(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkImageLayout finalLayout)
{
    VkAttachmentDescription attachment = {};
    attachment.format = format;
    attachment.samples = sampleCount;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = finalLayout;
    return attachment;
}
} // namespace prev::render::pass
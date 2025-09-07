#include "RenderPassBuilder.h"

#include "../../common/Logger.h"
#include "../../util/MathUtils.h"

namespace prev::render::pass {
RenderPassBuilder::RenderPassBuilder(VkDevice device)
    : m_device{ device }
{
}

RenderPassBuilder& RenderPassBuilder::AddColorAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkClearColorValue clearVal, const VkImageLayout finalLayout, const VkAttachmentLoadOp loadOp, const VkAttachmentStoreOp storeOp, const bool resolveAttachment)
{
    VkClearValue clearColor{};
    clearColor.color = clearVal;

    m_attachmentInfos.push_back(RenderPass::AttachmentInfo{ clearColor, format, sampleCount, finalLayout, loadOp, storeOp, resolveAttachment });

    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddDepthAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkClearDepthStencilValue clearVal, const VkImageLayout finalLayout, const VkAttachmentLoadOp loadOp, const VkAttachmentStoreOp storeOp, const bool resolveAttachment)
{
    VkClearValue clearDepth{};
    clearDepth.depthStencil = clearVal;

    m_attachmentInfos.push_back(RenderPass::AttachmentInfo{ clearDepth, format, sampleCount, finalLayout, loadOp, storeOp, resolveAttachment });

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

RenderPassBuilder& RenderPassBuilder::SetViewCount(const uint32_t viewCount)
{
    m_viewCount = viewCount;

    return *this;
}

std::unique_ptr<RenderPass> RenderPassBuilder::Build() const
{
    std::vector<VkAttachmentDescription> attachments(m_attachmentInfos.size());
    for (size_t i = 0; i < m_attachmentInfos.size(); ++i) {
        const auto& attachmentInfo{ m_attachmentInfos[i] };
        attachments[i] = CreateAttachmentDescription(attachmentInfo.format, attachmentInfo.sampleCount, attachmentInfo.finalLayout, attachmentInfo.loadOp, attachmentInfo.storeOp);
    }

    std::vector<SubPass> subpasses;
    for (const auto& subPassCreateInfo : m_subPassCreateInfos) {
        SubPass subpass{ attachments };
        subpass.UseAttachments(subPassCreateInfo.attachmentIndices);
        subpass.UseResolveAttachments(subPassCreateInfo.resolveIndices);
        subpasses.emplace_back(subpass);
    }

    std::vector<VkSubpassDescription> subpassDescriptions;
    subpassDescriptions.insert(subpassDescriptions.end(), subpasses.begin(), subpasses.end());

    VkRenderPassCreateInfo renderPassCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(m_dependencies.size());
    renderPassCreateInfo.pDependencies = m_dependencies.data();
    renderPassCreateInfo.pNext = nullptr;
#ifdef ENABLE_XR
    const uint32_t viewMask{ prev::util::math::SetBits<uint32_t>(m_viewCount) };
    const uint32_t correlationMask{ prev::util::math::SetBits<uint32_t>(m_viewCount) };

    VkRenderPassMultiviewCreateInfo renderPassMultiviewCreateInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO };
    renderPassMultiviewCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
    renderPassMultiviewCreateInfo.pViewMasks = &viewMask;
    renderPassMultiviewCreateInfo.correlationMaskCount = 1;
    renderPassMultiviewCreateInfo.pCorrelationMasks = &correlationMask;
    renderPassMultiviewCreateInfo.pNext = nullptr;

    if (m_viewCount > 1) {
        renderPassCreateInfo.pNext = &renderPassMultiviewCreateInfo;
    }
#endif

    VkRenderPass vkRenderPass{};
    VKERRCHECK(vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &vkRenderPass));

    auto renderPass{ std::unique_ptr<RenderPass>(new RenderPass(m_device, vkRenderPass, m_attachmentInfos, subpasses, m_dependencies)) };

    LOGI("Renderpass created");

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
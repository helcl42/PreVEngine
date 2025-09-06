#include "RenderPass.h"

#include "../../common/Logger.h"

#include <prev/core/Formats.h>

#include <algorithm>

namespace prev::render::pass {
RenderPass::RenderPass(const VkDevice device, const VkRenderPass renderPass, const std::vector<AttachmentInfo>& attachmentInfos, const std::vector<SubPass>& subpasses, const std::vector<VkSubpassDependency>& dependencies)
    : m_device{ device }
    , m_renderPass{ renderPass }
    , m_attachmentInfos{ attachmentInfos }
    , m_subpasses{ subpasses }
    , m_dependencies{ dependencies }
{
    m_sampleCount = VK_SAMPLE_COUNT_1_BIT;
    for (const auto& attachmentInfo : attachmentInfos) {
        m_clearValues.push_back(attachmentInfo.clearValue);
        if (prev::core::format::HasDepthComponent(attachmentInfo.format)) {
            m_depthFormats.push_back(attachmentInfo.format);
        } else {
            m_colorFormats.push_back(attachmentInfo.format);
            if (!attachmentInfo.resolveAttachment) {
                m_nonResolveColorFormats.push_back(attachmentInfo.format);
            }
        }
        m_sampleCount = std::max(m_sampleCount, attachmentInfo.sampleCount);
    }
}

RenderPass::~RenderPass()
{
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    m_renderPass = VK_NULL_HANDLE;

    LOGI("Renderpass destroyed");
}

void RenderPass::Begin(const VkFramebuffer frameBuffer, const VkCommandBuffer commandBuffer, const VkRect2D& renderArea, const VkSubpassContents contents)
{
    VkRenderPassBeginInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = frameBuffer;
    renderPassInfo.renderArea = renderArea;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(m_clearValues.size());
    renderPassInfo.pClearValues = m_clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, contents);
}

void RenderPass::End(const VkCommandBuffer commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer);
}

const std::vector<RenderPass::AttachmentInfo>& RenderPass::GetAttachments() const
{
    return m_attachmentInfos;
}

const std::vector<SubPass>& RenderPass::GetSubPasses() const
{
    return m_subpasses;
}

const std::vector<VkClearValue>& RenderPass::GetClearValues() const
{
    return m_clearValues;
}

VkFormat RenderPass::GetColorFormat(const uint32_t attachmentIndex) const
{
    if (attachmentIndex >= static_cast<uint32_t>(m_colorFormats.size())) {
        return VkFormat::VK_FORMAT_UNDEFINED;
    }
    return m_colorFormats[attachmentIndex];
}

const std::vector<VkFormat>& RenderPass::GetColorFormats(const bool includeResolveAttachments) const
{
    if (includeResolveAttachments) {
        return m_colorFormats;
    } else {
        return m_nonResolveColorFormats;
    }
}

VkFormat RenderPass::GetDepthFormat(const uint32_t attachmentIndex) const
{
    if (attachmentIndex >= static_cast<uint32_t>(m_depthFormats.size())) {
        return VkFormat::VK_FORMAT_UNDEFINED;
    }
    return m_depthFormats[attachmentIndex];
}

VkSampleCountFlagBits RenderPass::GetSamplesCount() const
{
    return m_sampleCount;
}

const std::vector<VkSubpassDependency>& RenderPass::GetSubPassDependencies() const
{
    return m_dependencies;
}

RenderPass::operator VkRenderPass() const
{
    return m_renderPass;
}
} // namespace prev::render::pass
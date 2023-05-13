#include "RenderPass.h"

namespace prev::render::pass {
RenderPass::RenderPass(VkDevice device)
    : m_device(device)
    , m_renderPass(VK_NULL_HANDLE)
{
}

RenderPass::~RenderPass()
{
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    m_renderPass = VK_NULL_HANDLE;

    LOGI("Renderpass destroyed\n");
}

void RenderPass::Begin(const VkFramebuffer frameBuffer, const VkCommandBuffer commandBuffer, const VkRect2D& renderArea, const VkSubpassContents contents)
{
    VkRenderPassBeginInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
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

VkFormat RenderPass::GetColorFormat(const int attachmentIndex) const
{
    if (attachmentIndex >= 0 && attachmentIndex < static_cast<int>(m_colorFormats.size())) {
        return m_colorFormats.at(attachmentIndex);
    }
    return VkFormat::VK_FORMAT_UNDEFINED;
}

std::vector<VkFormat> RenderPass::GetColorFormats(const bool includeResolveAttachments) const
{
    if (includeResolveAttachments) {
        return m_colorFormats;
    } else {
        std::vector<VkFormat> result;
        for (size_t i = 0; i < m_colorFormats.size(); ++i) {
            if (!m_resolveAttachments[i]) {
                result.push_back(m_colorFormats[i]);
            }
        }
        return result;
    }
}

VkFormat RenderPass::GetDepthFormat(const int attachmentIndex) const
{
    if (attachmentIndex >= 0 && attachmentIndex < static_cast<int>(m_depthFormats.size())) {
        return m_depthFormats.at(attachmentIndex);
    }
    return VkFormat::VK_FORMAT_UNDEFINED;
}

VkSampleCountFlagBits RenderPass::GetSamplesCount() const
{
    VkSampleCountFlagBits samplesCount{ VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT };
    for (const auto& a : m_attachments) {
        if (a.samples > samplesCount) {
            samplesCount = a.samples;
        }
    }
    return samplesCount;
}

const std::vector<VkClearValue>& RenderPass::GetClearValues() const
{
    return m_clearValues;
}

const std::vector<SubPass>& RenderPass::GetSubPasses() const
{
    return m_subpasses;
}

const std::vector<VkAttachmentDescription>& RenderPass::GetAttachments() const
{
    return m_attachments;
}

const std::vector<VkSubpassDependency>& RenderPass::GetSubPassDependencies() const
{
    return m_dependencies;
}

RenderPass::operator VkRenderPass() const
{
    ASSERT(m_renderPass, "Renderpass has to be created first: Use RenderPassBuilder.\n");

    return m_renderPass;
}
} // namespace prev::render::pass
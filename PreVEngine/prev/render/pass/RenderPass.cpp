#include "RenderPass.h"

namespace prev::render::pass {
RenderPass::RenderPass(VkDevice device)
    : m_device(device)
    , m_renderPass(VK_NULL_HANDLE)
{
}
RenderPass::~RenderPass()
{
    Destroy();
}

VkAttachmentDescription RenderPass::CreateAttachmentDescription(VkFormat format, VkImageLayout finalLayout, VkSampleCountFlagBits sampleCount)
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

uint32_t RenderPass::AddColorAttachment(VkFormat format, VkClearColorValue clearVal, VkImageLayout finalLayout, VkSampleCountFlagBits sampleCount)
{
    m_clearValues.push_back({});
    m_clearValues.back().color = clearVal;

    if (finalLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        m_surfaceFormat = format;
    }

    m_attachments.push_back(CreateAttachmentDescription(format, finalLayout, sampleCount));

    return static_cast<uint32_t>(m_attachments.size() - 1);
}

uint32_t RenderPass::AddDepthAttachment(VkFormat format, VkClearDepthStencilValue clearVal, VkSampleCountFlagBits sampleCount)
{
    ASSERT(m_depthFormat == VK_FORMAT_UNDEFINED, "Renderpass can't have more than one depth buffer. ");
    m_depthFormat = format;

    m_clearValues.push_back({});
    m_clearValues.back().depthStencil = clearVal;

    m_attachments.push_back(CreateAttachmentDescription(format, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, sampleCount));

    return static_cast<uint32_t>(m_attachments.size() - 1);
}

SubPass& RenderPass::AddSubpass(const std::vector<uint32_t>& attachmentIndexes)
{
    m_subpasses.push_back(SubPass(*this));

    SubPass& subpass = m_subpasses.back();
    for (const auto& i : attachmentIndexes) {
        subpass.UseAttachment(i);
    }

    return subpass;
}

void RenderPass::AddSubpassDependency(const std::vector<VkSubpassDependency>& dependencies)
{
    m_dependencies.insert(m_dependencies.end(), dependencies.cbegin(), dependencies.cend());
}

void RenderPass::Create()
{
    ASSERT(!m_renderPass, "Renderpass cannot be modified after its been linked to swapchain or pipeline.\n");

    if (m_renderPass != VK_NULL_HANDLE) {
        return;
    }

    std::vector<VkSubpassDescription> vkSubs;
    vkSubs.insert(vkSubs.end(), m_subpasses.begin(), m_subpasses.end());

    VkRenderPassCreateInfo renderPassCreateInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
    renderPassCreateInfo.pAttachments = m_attachments.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(vkSubs.size());
    renderPassCreateInfo.pSubpasses = vkSubs.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(m_dependencies.size());
    renderPassCreateInfo.pDependencies = m_dependencies.data();
    VKERRCHECK(vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &m_renderPass));

    LOGI("Renderpass created\n");
}

void RenderPass::Destroy()
{
    if (m_renderPass == VK_NULL_HANDLE) {
        return;
    }

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

VkFormat RenderPass::GetSurfaceFormat() const
{
    return m_surfaceFormat;
}

VkFormat RenderPass::GetDepthFormat() const
{
    return m_depthFormat;
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

RenderPass::operator VkRenderPass()
{
    if (m_renderPass == VK_NULL_HANDLE) {
        Create();
    }
    return m_renderPass;
}
} // namespace prev::render::pass
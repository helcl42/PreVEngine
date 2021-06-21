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

VkAttachmentDescription RenderPass::CreateAttachmentDescription(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkImageLayout finalLayout)
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

uint32_t RenderPass::AddColorAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkClearColorValue clearVal, const VkImageLayout finalLayout)
{
    VkClearValue clearColor{};
    clearColor.color = clearVal;
    m_clearValues.push_back(clearColor);

    m_surfaceFormats.push_back(format);
    m_attachments.push_back(CreateAttachmentDescription(format, sampleCount, finalLayout));

    return static_cast<uint32_t>(m_attachments.size() - 1);
}

uint32_t RenderPass::AddDepthAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkClearDepthStencilValue clearVal)
{
    ASSERT(m_depthFormat == VK_FORMAT_UNDEFINED, "Renderpass cannot be modified after its been linked to swapchain or pipeline.\n");

    m_depthFormat = format;

    VkClearValue clearDepth{};
    clearDepth.depthStencil = clearVal;
    m_clearValues.push_back(clearDepth);

    m_attachments.push_back(CreateAttachmentDescription(format, sampleCount, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL));

    return static_cast<uint32_t>(m_attachments.size() - 1);
}

SubPass& RenderPass::AddSubpass(const std::vector<uint32_t>& attachmentIndexes, const std::vector<uint32_t>& resolveIndices)
{
    m_subpasses.push_back(SubPass(*this));

    auto& subpass{ m_subpasses.back() };
    subpass.UseAttachments(attachmentIndexes);
    subpass.UseResolveAttachments(resolveIndices);

    return subpass;
}

void RenderPass::AddSubpassDependencies(const std::vector<VkSubpassDependency>& dependencies)
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

VkFormat RenderPass::GetColorFormat(const int attachmentIndex) const
{
    if (attachmentIndex >= 0 && attachmentIndex < static_cast<int>(m_surfaceFormats.size())) {
        return m_surfaceFormats.at(attachmentIndex);
    }
    return VkFormat::VK_FORMAT_UNDEFINED;
}

VkFormat RenderPass::GetDepthFormat() const
{
    return m_depthFormat;
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
    ASSERT(m_renderPass, "Renderpass has to be created first.\n");

    return m_renderPass;
}

VkRenderPass RenderPass::GetNativeHandle() const
{
    return m_renderPass;
}
} // namespace prev::render::pass
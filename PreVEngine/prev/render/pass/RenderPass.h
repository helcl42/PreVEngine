#ifndef __RENDER_PASS_H__
#define __RENDER_PASS_H__

#include "SubPass.h"

#include "../../core/Core.h"

#include <vector>

namespace prev::render::pass {
class RenderPass final {
public:
    RenderPass(
        const VkDevice device,
        const VkRenderPass renderPass,
        const std::vector<VkAttachmentDescription>& attachments,
        const std::vector<SubPass>& subpasses,
        const std::vector<VkClearValue>& clearValues,
        const std::vector<VkFormat>& colorFormats,
        const std::vector<bool>& colorResolveAttachments,
        const std::vector<VkFormat>& depthFormats,
        const std::vector<bool>& depthResolveAttachments,
        const std::vector<VkSubpassDependency>& dependencies);

    ~RenderPass();

public:
    void Begin(const VkFramebuffer frambuffer, const VkCommandBuffer commandBuffer, const VkRect2D& renderArea, const VkSubpassContents contents = VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

    void End(VkCommandBuffer commadBuffer);

public:
    const std::vector<VkAttachmentDescription>& GetAttachments() const;

    const std::vector<SubPass>& GetSubPasses() const;

    const std::vector<VkClearValue>& GetClearValues() const;

    VkFormat GetColorFormat(const int attachmentIndex = 0) const;

    std::vector<VkFormat> GetColorFormats(const bool includeResolveAttachments = false) const;

    VkFormat GetDepthFormat(const int attachmentIndex = 0) const;

    VkSampleCountFlagBits GetSamplesCount() const;

    const std::vector<VkSubpassDependency>& GetSubPassDependencies() const;

public:
    operator VkRenderPass() const;

private:
    VkDevice m_device;

    VkRenderPass m_renderPass;

    std::vector<VkAttachmentDescription> m_attachments;

    std::vector<SubPass> m_subpasses;

    std::vector<VkClearValue> m_clearValues;

    std::vector<VkFormat> m_colorFormats;

    std::vector<bool> m_colorResolveAttachments;

    std::vector<VkFormat> m_depthFormats;

    std::vector<bool> m_depthResolveAttachments;

    std::vector<VkSubpassDependency> m_dependencies;
};
} // namespace prev::render::pass

#endif

#ifndef __RENDER_PASS_H__
#define __RENDER_PASS_H__

#include "SubPass.h"

#include "../../core/Core.h"

#include <vector>

namespace prev::render::pass {
class RenderPassBuilder;

class RenderPass final {
public:
    struct AttachmentInfo {
        VkClearValue clearValue;
        VkFormat format;
        VkSampleCountFlagBits sampleCount;
        VkImageLayout finalLayout;
        VkAttachmentLoadOp loadOp;
        VkAttachmentStoreOp storeOp;
        bool resolveAttachment;
    };

public:
    RenderPass(const VkDevice device, const VkRenderPass renderPass, const std::vector<AttachmentInfo>& attachmentInfos, const std::vector<SubPass>& subpasses, const std::vector<VkSubpassDependency>& dependencies);

    ~RenderPass();

public:
    void Begin(const VkFramebuffer frambuffer, const VkCommandBuffer commandBuffer, const VkRect2D& renderArea, const VkSubpassContents contents = VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

    void End(VkCommandBuffer commadBuffer);

public:
    const std::vector<AttachmentInfo>& GetAttachments() const;

    const std::vector<SubPass>& GetSubPasses() const;

    const std::vector<VkClearValue>& GetClearValues() const;

    VkFormat GetColorFormat(const uint32_t attachmentIndex = 0) const;

    const std::vector<VkFormat>& GetColorFormats(const bool includeResolveAttachments = false) const;

    VkFormat GetDepthFormat(const uint32_t attachmentIndex = 0) const;

    VkSampleCountFlagBits GetSamplesCount() const;

    const std::vector<VkSubpassDependency>& GetSubPassDependencies() const;

public:
    operator VkRenderPass() const;

public:
    friend class RenderPassBuilder;

private:
    VkDevice m_device;

    VkRenderPass m_renderPass;

    std::vector<AttachmentInfo> m_attachmentInfos;

    std::vector<SubPass> m_subpasses;

    std::vector<VkSubpassDependency> m_dependencies;

    std::vector<VkClearValue> m_clearValues;

    std::vector<VkFormat> m_colorFormats;

    std::vector<VkFormat> m_nonResolveColorFormats;

    std::vector<VkFormat> m_depthFormats;

    VkSampleCountFlagBits m_sampleCount;
};
} // namespace prev::render::pass

#endif

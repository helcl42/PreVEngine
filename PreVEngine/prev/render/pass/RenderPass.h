#ifndef __RENDER_PASS_H__
#define __RENDER_PASS_H__

#include "../../core/instance/Validation.h"
#include "SubPass.h"

#include <vector>

namespace prev::render::pass {
class RenderPassBuilder;

class RenderPass final {
public:
    RenderPass(VkDevice device); // TODO - this should be private - accesible from builder only!

    ~RenderPass();

public:
    void Begin(const VkFramebuffer frambuffer, const VkCommandBuffer commandBuffer, const VkRect2D& renderArea, const VkSubpassContents contents = VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

    void End(VkCommandBuffer commadBuffer);

public:
    VkFormat GetFormat(const int attachmentIndex = 0) const;

    VkFormat GetDepthFormat(const int attachmentIndex = 0) const;

    VkSampleCountFlagBits GetSamplesCount() const;

    const std::vector<VkClearValue>& GetClearValues() const;

    const std::vector<SubPass>& GetSubPasses() const;

    const std::vector<VkAttachmentDescription>& GetAttachments() const;

    const std::vector<VkSubpassDependency>& GetSubPassDependencies() const;

public:
    operator VkRenderPass() const;

    VkRenderPass GetNativeHandle() const;

private:
    friend class RenderPassBuilder;

private:
    VkDevice m_device;

    VkRenderPass m_renderPass;

    std::vector<VkClearValue> m_clearValues;

    std::vector<VkFormat> m_colorFormats;

    std::vector<VkFormat> m_depthFormats;

    std::vector<SubPass> m_subpasses;

    std::vector<VkAttachmentDescription> m_attachments;

    std::vector<VkSubpassDependency> m_dependencies;
};
} // namespace prev::render::pass

#endif

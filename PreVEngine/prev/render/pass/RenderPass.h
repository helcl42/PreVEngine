#ifndef __RENDER_PASS_H__
#define __RENDER_PASS_H__

#include "../../core/instance/Validation.h"
#include "SubPass.h"

#include <vector>

namespace prev::render::pass {
class RenderPass final {
public:
    RenderPass(VkDevice device);

    ~RenderPass();

private:
    static VkAttachmentDescription CreateAttachmentDescription(VkFormat format, VkSampleCountFlagBits sampleCount, VkImageLayout finalLayout);

public:
    uint32_t AddColorAttachment(VkFormat format, VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, VkClearColorValue clearVal = {}, VkImageLayout finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    uint32_t AddDepthAttachment(VkFormat format, VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, VkClearDepthStencilValue clearVal = { 1.0f, 0 });

    SubPass& AddSubpass(const std::vector<uint32_t>& attachmentIndexes = {}, const std::vector<uint32_t>& resolveIndices = {});

    void AddSubpassDependencies(const std::vector<VkSubpassDependency>& dependencies);

    void Create();

    void Destroy();

    void Begin(const VkFramebuffer frambuffer, const VkCommandBuffer commandBuffer, const VkRect2D& renderArea, const VkSubpassContents contents = VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

    void End(VkCommandBuffer commadBuffer);

public:
    VkFormat GetSurfaceFormat() const;

    VkFormat GetDepthFormat() const;

    VkSampleCountFlagBits GetSamplesCount() const;

    const std::vector<VkClearValue>& GetClearValues() const;

    const std::vector<SubPass>& GetSubPasses() const;

    const std::vector<VkAttachmentDescription>& GetAttachments() const;

    const std::vector<VkSubpassDependency>& GetSubPassDependencies() const;

public:
    operator VkRenderPass();

private:
    VkDevice m_device;

    VkRenderPass m_renderPass{ VK_NULL_HANDLE };

    VkFormat m_surfaceFormat{ VkFormat::VK_FORMAT_UNDEFINED };

    VkFormat m_depthFormat{ VkFormat::VK_FORMAT_UNDEFINED };

    std::vector<VkClearValue> m_clearValues;

    std::vector<SubPass> m_subpasses;

    std::vector<VkAttachmentDescription> m_attachments;

    std::vector<VkSubpassDependency> m_dependencies;
};
} // namespace prev::render::pass

#endif

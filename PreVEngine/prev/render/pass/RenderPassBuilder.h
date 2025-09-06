#ifndef __RENDER_PASS_BUILDER_H__
#define __RENDER_PASS_BUILDER_H__

#include "RenderPass.h"

#include "../../core/Core.h"

#include <memory>
#include <vector>

namespace prev::render::pass {
class RenderPassBuilder final {
public:
    RenderPassBuilder(VkDevice device);

    ~RenderPassBuilder() = default;

public:
    RenderPassBuilder& AddColorAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, const VkClearColorValue clearVal = {}, const VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, const VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, const VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE, const bool resolveAttachment = false);

    RenderPassBuilder& AddDepthAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, const VkClearDepthStencilValue clearVal = { MAX_DEPTH, 0 }, const VkImageLayout finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, const VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR, const VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE, const bool resolveAttachment = false);

    RenderPassBuilder& AddSubpass(const std::vector<uint32_t>& attachmentIndices = {}, const std::vector<uint32_t>& resolveIndices = {});

    RenderPassBuilder& AddSubpassDependencies(const std::vector<VkSubpassDependency>& dependencies);

    RenderPassBuilder& SetViewCount(const uint32_t viewCount);

    std::unique_ptr<RenderPass> Build() const;

private:
    static VkAttachmentDescription CreateAttachmentDescription(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkImageLayout finalLayout, const VkAttachmentLoadOp loadOp, const VkAttachmentStoreOp storeOp);

private:
    struct SubPassCreateInfo {
        std::vector<uint32_t> attachmentIndices;
        std::vector<uint32_t> resolveIndices;
    };

private:
    VkDevice m_device;

    uint32_t m_viewCount{ 1 };

    std::vector<RenderPass::AttachmentInfo> m_attachmentInfos;

    std::vector<SubPassCreateInfo> m_subPassCreateInfos;

    std::vector<VkSubpassDependency> m_dependencies;
};
} // namespace prev::render::pass

#endif // !__RENDER_PASS_BUILDER_H__

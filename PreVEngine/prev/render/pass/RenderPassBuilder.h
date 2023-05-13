#ifndef __RENDER_PASS_BUILDER_H__
#define __RENDER_PASS_BUILDER_H__

#include "../../core/instance/Validation.h"
#include "RenderPass.h"

#include <memory>
#include <vector>

namespace prev::render::pass {
class RenderPassBuilder final {
public:
    RenderPassBuilder(VkDevice device);

    RenderPassBuilder() = default;

public:
    RenderPassBuilder& AddColorAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, const VkClearColorValue clearVal = {}, const VkImageLayout finalLayout = VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, const bool resolveAttachment = false);

    RenderPassBuilder& AddDepthAttachment(const VkFormat format, const VkSampleCountFlagBits sampleCount = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, const VkClearDepthStencilValue clearVal = { 1.0f, 0 }, const bool resolveAttachment = false);

    RenderPassBuilder& AddSubpass(const std::vector<uint32_t>& attachmentIndexes = {}, const std::vector<uint32_t>& resolveIndices = {});

    RenderPassBuilder& AddSubpassDependencies(const std::vector<VkSubpassDependency>& dependencies);

    std::unique_ptr<RenderPass> Build() const;

private:
    static VkAttachmentDescription CreateAttachmentDescription(const VkFormat format, const VkSampleCountFlagBits sampleCount, const VkImageLayout finalLayout);

private:
    struct SubPassCreateInfo {
        std::vector<uint32_t> attachmentIndexes;
        std::vector<uint32_t> resolveIndices;
    };

    struct AttachmentCreateInfo {
        VkClearValue clearValue;
        VkFormat format;
        VkSampleCountFlagBits sampleCount;
        VkImageLayout finalLayout;
        bool resolveAttachment;
    };

private:
    VkDevice m_device;

    std::vector<AttachmentCreateInfo> m_attachmentInfos;

    std::vector<SubPassCreateInfo> m_subPassCreateInfos;

    std::vector<VkSubpassDependency> m_dependencies;

    static const VkImageLayout DEFAULT_DEPTH_LAYOUT{ VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL };
};
} // namespace prev::render::pass

#endif // !__RENDER_PASS_BUILDER_H__

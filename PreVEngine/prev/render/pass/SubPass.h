#ifndef __SUB_PASS_H__
#define __SUB_PASS_H__

#include "../../core/Core.h"

#include <vector>

namespace prev::render::pass {
class RenderPass;

class SubPass final {
public:
    explicit SubPass(const RenderPass& renderpass);

    ~SubPass() = default;

public:
    void UseAttachment(const uint32_t attachmentIndex); // for write

    void UseAttachments(const std::vector<uint32_t>& attachmentIndices = {});

    void UseResolveAttachment(const uint32_t attachmentIndex); // for write

    void UseResolveAttachments(const std::vector<uint32_t>& attachmentIndices = {});

    void InputAttachment(const uint32_t attachmentIndex); // for read

    void InputAttachments(const std::vector<uint32_t>& attachmentIndices = {});

public:
    operator VkSubpassDescription();

private:
    const RenderPass& m_renderPass;

    std::vector<VkAttachmentReference> m_inputReferences;

    std::vector<VkAttachmentReference> m_colorReferences;

    std::vector<VkAttachmentReference> m_resolveReferences;

    VkAttachmentReference m_depthReference{};
};
} // namespace prev::render::pass

#endif
#ifndef __SUB_PASS_H__
#define __SUB_PASS_H__

#include "../../core/instance/Validation.h"

#include <vector>

namespace PreVEngine {
class RenderPass;

class SubPass final {
public:
    SubPass(RenderPass& renderpass);

    ~SubPass() = default;

public:
    void UseAttachment(uint32_t attachmentIndex); // for write

    void UseAttachments(const std::vector<uint32_t>& attachmentIndexes = {});

    void InputAttachment(uint32_t attachmentIndex); // for read

    void InputAttachments(const std::vector<uint32_t>& attachmentIndices = {});

public:
    operator VkSubpassDescription();

private:
    RenderPass& m_renderPass;

    std::vector<VkAttachmentReference> m_inputReferences;

    std::vector<VkAttachmentReference> m_colorReferences;

    VkAttachmentReference m_depthReference = { UINT32_MAX, VK_IMAGE_LAYOUT_UNDEFINED };
};
} // namespace PreVEngine

#endif
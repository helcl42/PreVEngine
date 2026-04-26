#ifndef __RENDER_PASS_BUILDER_H__
#define __RENDER_PASS_BUILDER_H__

#include "RenderPass.h"

#include "../../core/Core.h"

#include <memory>
#include <vector>

namespace prev::render::pass {
class RenderPassBuilder final {
public:
    RenderPassBuilder(GfxDevice device);

    ~RenderPassBuilder() = default;

public:
    RenderPassBuilder& AddColorAttachment(const GfxFormat format, const GfxSampleCount sampleCount = GFX_SAMPLE_COUNT_1, const GfxColor clearVal = {}, const GfxTextureLayout finalLayout = GFX_TEXTURE_LAYOUT_PRESENT_SRC, const GfxLoadOp loadOp = GFX_LOAD_OP_CLEAR, const GfxStoreOp storeOp = GFX_STORE_OP_STORE, const bool resolveAttachment = false);

    RenderPassBuilder& AddDepthAttachment(const GfxFormat format, const GfxSampleCount sampleCount = GFX_SAMPLE_COUNT_1, const float depthClearVal = MAX_DEPTH, const uint32_t stencilClearVal = 0, const GfxTextureLayout finalLayout = GFX_TEXTURE_LAYOUT_DEPTH_STENCIL_READ_ONLY, const GfxLoadOp loadOp = GFX_LOAD_OP_CLEAR, const GfxStoreOp storeOp = GFX_STORE_OP_STORE, const bool resolveAttachment = false);

    RenderPassBuilder& AddSubpass(const std::vector<uint32_t>& attachmentIndices = {}, const std::vector<uint32_t>& resolveIndices = {});

    RenderPassBuilder& SetViewCount(const uint32_t viewCount);

    std::unique_ptr<RenderPass> Build() const;

private:
    void Validate() const;

private:
    struct SubPassCreateInfo {
        std::vector<uint32_t> attachmentIndices;
        std::vector<uint32_t> resolveIndices;
    };

private:
    GfxDevice m_device;

    uint32_t m_viewCount{ 1 };

    std::vector<RenderPass::AttachmentInfo> m_attachmentInfos;

    std::vector<SubPassCreateInfo> m_subPassCreateInfos;
};
} // namespace prev::render::pass

#endif // !__RENDER_PASS_BUILDER_H__

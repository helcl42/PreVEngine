#ifndef __RENDER_PASS_H__
#define __RENDER_PASS_H__

#include "../../core/Core.h"

#include <vector>

namespace prev::render::pass {
class RenderPassBuilder;

class RenderPass final {
public:
    struct AttachmentInfo {
        GfxColor colorClearValue{};
        float depthClearValue{ 1.0f };
        uint32_t stencilClearValue{ 0 };
        GfxFormat format;
        GfxSampleCount sampleCount;
        GfxTextureLayout finalLayout;
        GfxLoadOp loadOp;
        GfxStoreOp storeOp;
        bool resolveAttachment;
    };

public:
    RenderPass(GfxDevice device, GfxRenderPass renderPass, GfxFormat colorFormat, GfxFormat depthFormat, GfxSampleCount sampleCount);

    RenderPass(GfxDevice device, GfxRenderPass renderPass, const std::vector<AttachmentInfo>& attachmentInfos);

    ~RenderPass();

public:
    void Begin(GfxFramebuffer framebuffer, GfxCommandEncoder commandEncoder);

    void End();

    void SetOcclusionQuerySet(GfxQuerySet querySet);

    void SetTimestampQuerySet(GfxQuerySet querySet);

public:
    const std::vector<AttachmentInfo>& GetAttachments() const;

    GfxFormat GetColorFormat() const;

    GfxFormat GetDepthFormat() const;

    GfxSampleCount GetSampleCount() const;

    std::vector<GfxFormat> GetGfxColorFormats() const;

    GfxRenderPassEncoder GetEncoder() const;

public:
    operator GfxRenderPass() const;

public:
    friend class RenderPassBuilder;

private:
    GfxDevice m_gfxDevice{};

    GfxRenderPass m_gfxRenderPass{};

    GfxFormat m_gfxColorFormat{ GFX_FORMAT_UNDEFINED };

    GfxFormat m_gfxDepthFormat{ GFX_FORMAT_UNDEFINED };

    GfxSampleCount m_gfxSampleCount{ GFX_SAMPLE_COUNT_1 };

    std::vector<AttachmentInfo> m_attachmentInfos;

    GfxRenderPassEncoder m_activeEncoder{};

    GfxQuerySet m_occlusionQuerySet{};

    GfxQuerySet m_timestampQuerySet{};
};
} // namespace prev::render::pass

#endif // !__RENDER_PASS_H__

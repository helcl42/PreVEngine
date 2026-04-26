#include "RenderPass.h"

#include "../../common/Logger.h"
#include "../../core/Formats.h"

namespace prev::render::pass {
RenderPass::RenderPass(GfxDevice device, GfxRenderPass renderPass, GfxFormat colorFormat, GfxFormat depthFormat, GfxSampleCount sampleCount)
    : m_gfxDevice{ device }
    , m_gfxRenderPass{ renderPass }
    , m_gfxColorFormat{ colorFormat }
    , m_gfxDepthFormat{ depthFormat }
    , m_gfxSampleCount{ sampleCount }
{
}

RenderPass::RenderPass(GfxDevice device, GfxRenderPass renderPass, const std::vector<AttachmentInfo>& attachmentInfos)
    : m_gfxDevice{ device }
    , m_gfxRenderPass{ renderPass }
    , m_attachmentInfos{ attachmentInfos }
{
    for (const auto& info : attachmentInfos) {
        if (prev::core::format::HasDepthComponent(info.format)) {
            m_gfxDepthFormat = info.format;
        } else if (!info.resolveAttachment) {
            m_gfxColorFormat = info.format;
        }
        m_gfxSampleCount = static_cast<GfxSampleCount>(std::max(static_cast<int>(m_gfxSampleCount), static_cast<int>(info.sampleCount)));
    }
}

RenderPass::~RenderPass()
{
    if (m_gfxRenderPass) {
        gfxRenderPassDestroy(m_gfxRenderPass);
    }
    LOGI("Renderpass destroyed");
}

void RenderPass::Begin(GfxFramebuffer framebuffer, GfxCommandEncoder commandEncoder)
{
    std::vector<GfxColor> colorClearValues;
    float depthClearValue{ 1.0f };
    uint32_t stencilClearValue{ 0 };

    if (!m_attachmentInfos.empty()) {
        for (const auto& info : m_attachmentInfos) {
            if (prev::core::format::HasDepthComponent(info.format)) {
                depthClearValue = info.depthClearValue;
                stencilClearValue = info.stencilClearValue;
            } else if (!info.resolveAttachment) {
                colorClearValues.push_back(info.colorClearValue);
            }
        }
    } else if (m_gfxColorFormat != GFX_FORMAT_UNDEFINED) {
        // Fallback for render passes created without AttachmentInfo (e.g. default MSAA pass)
        colorClearValues.push_back({ 0.0f, 0.0f, 0.0f, 1.0f });
    }

    GfxRenderPassBeginDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_RENDER_PASS_BEGIN_DESCRIPTOR;
    desc.renderPass = m_gfxRenderPass;
    desc.framebuffer = framebuffer;
    desc.colorClearValues = colorClearValues.empty() ? nullptr : colorClearValues.data();
    desc.colorClearValueCount = static_cast<uint32_t>(colorClearValues.size());
    desc.depthClearValue = depthClearValue;
    desc.stencilClearValue = stencilClearValue;

    gfxCommandEncoderBeginRenderPass(commandEncoder, &desc, &m_activeEncoder);
}

void RenderPass::End()
{
    gfxRenderPassEncoderEnd(m_activeEncoder);
    m_activeEncoder = {};
}

const std::vector<RenderPass::AttachmentInfo>& RenderPass::GetAttachments() const
{
    return m_attachmentInfos;
}

GfxFormat RenderPass::GetColorFormat() const
{
    return m_gfxColorFormat;
}

GfxFormat RenderPass::GetDepthFormat() const
{
    return m_gfxDepthFormat;
}

GfxSampleCount RenderPass::GetSampleCount() const
{
    return m_gfxSampleCount;
}

RenderPass::operator GfxRenderPass() const
{
    return m_gfxRenderPass;
}

std::vector<GfxFormat> RenderPass::GetGfxColorFormats() const
{
    std::vector<GfxFormat> result;
    for (const auto& info : m_attachmentInfos) {
        if (!prev::core::format::HasDepthComponent(info.format) && !info.resolveAttachment) {
            result.push_back(info.format);
        }
    }
    if (result.empty() && m_gfxColorFormat != GFX_FORMAT_UNDEFINED) {
        result.push_back(m_gfxColorFormat);
    }
    return result;
}

GfxRenderPassEncoder RenderPass::GetEncoder() const
{
    return m_activeEncoder;
}
} // namespace prev::render::pass

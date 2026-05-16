#include "RenderPass.h"

#include "../../common/Logger.h"
#include "../../core/Formats.h"

namespace prev::render::pass {
RenderPass::RenderPass(GfxDevice device, GfxRenderPass renderPass, const std::vector<AttachmentInfo>& attachmentInfos)
    : m_gfxDevice{ device }
    , m_gfxRenderPass{ renderPass }
    , m_attachmentInfos{ attachmentInfos }
{
    for (const auto& info : attachmentInfos) {
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

void RenderPass::Begin(GfxFramebuffer framebuffer, GfxCommandEncoder commandEncoder, bool bundleExecution)
{
    std::vector<GfxColor> colorClearValues;
    float depthClearValue{ 1.0f };
    uint32_t stencilClearValue{ 0 };

    for (const auto& info : m_attachmentInfos) {
        if (prev::core::format::HasDepthComponent(info.format)) {
            depthClearValue = info.depthClearValue;
            stencilClearValue = info.stencilClearValue;
        } else if (!info.resolveAttachment) {
            colorClearValues.push_back(info.colorClearValue);
        }
    }

    GfxRenderPassBeginDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_RENDER_PASS_BEGIN_DESCRIPTOR;
    desc.renderPass = m_gfxRenderPass;
    desc.framebuffer = framebuffer;
    desc.colorClearValues = colorClearValues.empty() ? nullptr : colorClearValues.data();
    desc.colorClearValueCount = static_cast<uint32_t>(colorClearValues.size());
    desc.depthClearValue = depthClearValue;
    desc.stencilClearValue = stencilClearValue;
    desc.occlusionQuerySet = m_occlusionQuerySet;
    desc.timestampQuerySet = m_timestampQuerySet;
    desc.bundleExecution = bundleExecution;

    gfxCommandEncoderBeginRenderPass(commandEncoder, &desc, &m_activeEncoder);
}

void RenderPass::End()
{
    gfxRenderPassEncoderEnd(m_activeEncoder);
    m_activeEncoder = {};
}

void RenderPass::SetOcclusionQuerySet(GfxQuerySet querySet)
{
    m_occlusionQuerySet = querySet;
}

void RenderPass::SetTimestampQuerySet(GfxQuerySet querySet)
{
    m_timestampQuerySet = querySet;
}

const std::vector<RenderPass::AttachmentInfo>& RenderPass::GetAttachments() const
{
    return m_attachmentInfos;
}

GfxFormat RenderPass::GetColorFormat(uint32_t index) const
{
    uint32_t current = 0;
    for (const auto& info : m_attachmentInfos) {
        if (!prev::core::format::HasDepthComponent(info.format) && !info.resolveAttachment) {
            if (current == index) {
                return info.format;
            }
            ++current;
        }
    }
    return GFX_FORMAT_UNDEFINED;
}

GfxFormat RenderPass::GetDepthFormat() const
{
    for (const auto& info : m_attachmentInfos) {
        if (prev::core::format::HasDepthComponent(info.format) && !info.resolveAttachment) {
            return info.format;
        }
    }
    return GFX_FORMAT_UNDEFINED;
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
    return result;
}

GfxRenderPassEncoder RenderPass::GetEncoder() const
{
    return m_activeEncoder;
}
} // namespace prev::render::pass

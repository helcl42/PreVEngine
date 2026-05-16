#include "RenderPassBuilder.h"

#include "../../common/Logger.h"
#include "../../core/Formats.h"
#include "../../util/MathUtils.h"

#include <stdexcept>

namespace prev::render::pass {
RenderPassBuilder::RenderPassBuilder(GfxDevice device)
    : m_device{ device }
{
}

RenderPassBuilder& RenderPassBuilder::AddColorAttachment(const GfxFormat format, const GfxSampleCount sampleCount, const GfxColor clearVal, const GfxTextureLayout finalLayout, const GfxLoadOp loadOp, const GfxStoreOp storeOp, const bool resolveAttachment)
{
    RenderPass::AttachmentInfo info{};
    info.colorClearValue = clearVal;
    info.format = format;
    info.sampleCount = sampleCount;
    info.finalLayout = finalLayout;
    info.loadOp = loadOp;
    info.storeOp = storeOp;
    info.resolveAttachment = resolveAttachment;
    m_attachmentInfos.push_back(info);
    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddDepthAttachment(const GfxFormat format, const GfxSampleCount sampleCount, const float depthClearVal, const uint32_t stencilClearVal, const GfxTextureLayout finalLayout, const GfxLoadOp loadOp, const GfxStoreOp storeOp, const bool resolveAttachment)
{
    RenderPass::AttachmentInfo info{};
    info.depthClearValue = depthClearVal;
    info.stencilClearValue = stencilClearVal;
    info.format = format;
    info.sampleCount = sampleCount;
    info.finalLayout = finalLayout;
    info.loadOp = loadOp;
    info.storeOp = storeOp;
    info.resolveAttachment = resolveAttachment;
    m_attachmentInfos.push_back(info);
    return *this;
}

RenderPassBuilder& RenderPassBuilder::AddSubpass(const std::vector<uint32_t>& attachmentIndices, const std::vector<uint32_t>& resolveIndices)
{
    m_subPassCreateInfos.push_back({ attachmentIndices, resolveIndices });
    return *this;
}

RenderPassBuilder& RenderPassBuilder::SetViewCount(const uint32_t viewCount)
{
    m_viewCount = viewCount;
    return *this;
}

std::unique_ptr<RenderPass> RenderPassBuilder::Build() const
{
    // Classify attachments
    std::vector<size_t> colorIndices, resolveIndices, depthIndices, depthResolveIndices;
    for (size_t i = 0; i < m_attachmentInfos.size(); ++i) {
        const auto& info = m_attachmentInfos[i];
        if (prev::core::format::HasDepthComponent(info.format)) {
            if (info.resolveAttachment) {
                depthResolveIndices.push_back(i);
            } else {
                depthIndices.push_back(i);
            }
        } else if (info.resolveAttachment) {
            resolveIndices.push_back(i);
        } else {
            colorIndices.push_back(i);
        }
    }

    // Build resolve targets (keep alive for the descriptor call)
    std::vector<GfxRenderPassColorAttachmentTarget> resolveTargets(resolveIndices.size());
    for (size_t i = 0; i < resolveIndices.size(); ++i) {
        const auto& info = m_attachmentInfos[resolveIndices[i]];
        resolveTargets[i].format = info.format;
        resolveTargets[i].sampleCount = info.sampleCount;
        resolveTargets[i].ops = { info.loadOp, info.storeOp };
        resolveTargets[i].finalLayout = info.finalLayout;
    }

    // Build color attachments
    std::vector<GfxRenderPassColorAttachment> colorAttachments(colorIndices.size());
    for (size_t i = 0; i < colorIndices.size(); ++i) {
        const auto& info = m_attachmentInfos[colorIndices[i]];
        colorAttachments[i].target.format = info.format;
        colorAttachments[i].target.sampleCount = info.sampleCount;
        colorAttachments[i].target.ops = { info.loadOp, info.storeOp };
        colorAttachments[i].target.finalLayout = info.finalLayout;
        colorAttachments[i].resolveTarget = (i < resolveIndices.size()) ? &resolveTargets[i] : nullptr;
    }

    // Build depth/stencil attachment
    GfxRenderPassDepthStencilAttachment depthAttachment{};
    GfxRenderPassDepthStencilAttachmentTarget depthResolveTarget{};
    const bool hasDepth = !depthIndices.empty();
    if (hasDepth) {
        const auto& info = m_attachmentInfos[depthIndices[0]];
        depthAttachment.target.format = info.format;
        depthAttachment.target.sampleCount = info.sampleCount;
        depthAttachment.target.depthOps = { info.loadOp, info.storeOp };
        depthAttachment.target.stencilOps = { GFX_LOAD_OP_DONT_CARE, GFX_STORE_OP_DONT_CARE };
        depthAttachment.target.finalLayout = info.finalLayout;

        if (!depthResolveIndices.empty()) {
            const auto& resolveInfo = m_attachmentInfos[depthResolveIndices[0]];
            depthResolveTarget.format = resolveInfo.format;
            depthResolveTarget.sampleCount = resolveInfo.sampleCount;
            depthResolveTarget.depthOps = { resolveInfo.loadOp, resolveInfo.storeOp };
            depthResolveTarget.stencilOps = { GFX_LOAD_OP_DONT_CARE, GFX_STORE_OP_DONT_CARE };
            depthResolveTarget.finalLayout = resolveInfo.finalLayout;
            depthAttachment.resolveTarget = &depthResolveTarget;
        } else {
            depthAttachment.resolveTarget = nullptr;
        }
    }

    GfxRenderPassDescriptor desc{};
    desc.sType = GFX_STRUCTURE_TYPE_RENDER_PASS_DESCRIPTOR;
    desc.label = "RenderPass";
    desc.colorAttachments = colorAttachments.empty() ? nullptr : colorAttachments.data();
    desc.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
    desc.depthStencilAttachment = hasDepth ? &depthAttachment : nullptr;

    // Multiview
    GfxRenderPassMultiviewDescriptor multiviewDesc{};
    uint32_t viewMask = 0;
    uint32_t correlationMask = 0;
    if (m_viewCount > 1) {
        viewMask = prev::util::math::SetBits<uint32_t>(m_viewCount);
        correlationMask = viewMask;
        multiviewDesc.sType = GFX_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_DESCRIPTOR;
        multiviewDesc.pNext = nullptr;
        multiviewDesc.viewMask = viewMask;
        multiviewDesc.correlationMasks = &correlationMask;
        multiviewDesc.correlationMaskCount = 1;
        desc.pNext = &multiviewDesc;
    }

    GfxRenderPass renderPass{};
    if (gfxDeviceCreateRenderPass(m_device, &desc, &renderPass) != GFX_RESULT_SUCCESS) {
        throw std::runtime_error("Failed to create render pass");
    }

    LOGI("Renderpass created");

    return std::make_unique<RenderPass>(m_device, renderPass, m_attachmentInfos);
}

void RenderPassBuilder::Validate() const
{
    if (m_attachmentInfos.empty()) {
        throw std::runtime_error("Invalid RenderPass configuration: No attachments defined.");
    }
}
} // namespace prev::render::pass


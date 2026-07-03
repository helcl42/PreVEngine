#ifndef __ISWAPCHAIN_H__
#define __ISWAPCHAIN_H__

#include "../../core/Core.h"
#include "../FrameSubmitSync.h"

namespace prev::render::swapchain {
struct FrameContext {
    GfxFramebuffer frameBuffer{};
    GfxCommandEncoder commandEncoder{};
    uint32_t index{};
    uint32_t viewOffset{ 0 }; // first camera-view (eye) index this pass renders
    uint32_t viewCount{ 1 }; // views rendered in this pass: 1 (per-eye/mono) or N (multiview)
};

class ISwapchain {
public:
    virtual bool BeginFrame(FrameContext& outContext) = 0;

    virtual void BeginPass(FrameContext& outContext, uint32_t passIndex) = 0;

    virtual void EndPass(uint32_t passIndex) = 0;

    virtual void EndFrame(const FrameSubmitSync& submitSync) = 0;

    virtual void Print() const = 0;

    virtual GfxExtent2D GetExtent() const = 0;

    virtual uint32_t GetImageCount() const = 0;

    virtual uint32_t GetPassCount() const = 0;

public:
    virtual ~ISwapchain() = default;
};
} // namespace prev::render::swapchain

#endif

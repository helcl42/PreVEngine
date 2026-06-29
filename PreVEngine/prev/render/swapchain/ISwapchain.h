#ifndef __ISWAPCHAIN_H__
#define __ISWAPCHAIN_H__

#include "../../core/Core.h"
#include "../FrameSubmitSync.h"

namespace prev::render::swapchain {
struct FrameContext {
    GfxFramebuffer frameBuffer{};
    GfxCommandEncoder commandEncoder{};
    uint32_t index{};
};

class ISwapchain {
public:
    virtual bool BeginFrame(FrameContext& outContext) = 0;

    virtual void EndFrame(const FrameSubmitSync& submitSync) = 0;

    virtual void Print() const = 0;

    virtual GfxExtent2D GetExtent() const = 0;

    virtual uint32_t GetImageCount() const = 0;

public:
    virtual ~ISwapchain() = default;
};
} // namespace prev::render::swapchain

#endif

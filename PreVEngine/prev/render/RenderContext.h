#ifndef __RENDER_CONTEXT_H__
#define __RENDER_CONTEXT_H__

#include "../core/Core.h"

namespace prev::render {
struct RenderContext {
    GfxFramebuffer frameBuffer{};

    GfxCommandEncoder commandEncoder{};

    GfxRenderPassEncoder renderPassEncoder{};

    uint32_t frameInFlightIndex{};

    GfxScissorRect rect{};

    RenderContext(const GfxFramebuffer fb, const GfxCommandEncoder ce, const uint32_t frameIndex, const GfxScissorRect& r)
        : frameBuffer{ fb }
        , commandEncoder{ ce }
        , frameInFlightIndex{ frameIndex }
        , rect{ r }
    {
    }

    virtual ~RenderContext() = default;
};
} // namespace prev::render

#endif

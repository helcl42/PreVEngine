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

    uint32_t viewOffset{ 0 };

    uint32_t viewCount{ 1 };

    bool colorManaged{ false };

    RenderContext(const GfxFramebuffer fb, const GfxCommandEncoder ce, const uint32_t frameIndex, const GfxScissorRect& r, const uint32_t viewOffsetArg = 0, const uint32_t viewCountArg = 1, const bool colorManagedArg = false)
        : frameBuffer{ fb }
        , commandEncoder{ ce }
        , frameInFlightIndex{ frameIndex }
        , rect{ r }
        , viewOffset{ viewOffsetArg }
        , viewCount{ viewCountArg }
        , colorManaged{ colorManagedArg }
    {
    }

    virtual ~RenderContext() = default;
};
} // namespace prev::render

#endif

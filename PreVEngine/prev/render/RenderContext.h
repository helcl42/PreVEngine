#ifndef __RENDER_CONTEXT_H__
#define __RENDER_CONTEXT_H__

#include "../core/instance/Validation.h"

namespace prev::render {
struct RenderContext {
    VkFramebuffer frameBuffer{};

    VkCommandBuffer commandBuffer{};

    uint32_t frameInFlightIndex{};

    VkRect2D rect{};

    RenderContext(const VkFramebuffer fb, const VkCommandBuffer cb, const uint32_t frameIndex, const VkRect2D& r)
        : frameBuffer{ fb }
        , commandBuffer{ cb }
        , frameInFlightIndex{ frameIndex }
        , rect{ r }
    {
    }

    virtual ~RenderContext() = default;
};
} // namespace prev::render

#endif
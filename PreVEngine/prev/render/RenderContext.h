#ifndef __RENDER_CONTEXT_H__
#define __RENDER_CONTEXT_H__

#include "../core/instance/Validation.h"

namespace prev::render {
struct RenderContext {
    VkFramebuffer frameBuffer{};

    VkCommandBuffer commandBuffer{};

    uint32_t frameInFlightIndex{};

    VkRect2D rect{};
};
} // namespace prev::scene

#endif
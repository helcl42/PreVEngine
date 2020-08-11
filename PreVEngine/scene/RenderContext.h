#ifndef __RENDER_CONTEXT_H__
#define __RENDER_CONTEXT_H__

#include "../core/instance/Validation.h"

namespace PreVEngine {
struct RenderContext {
    VkFramebuffer frameBuffer;

    VkCommandBuffer commandBuffer;

    uint32_t frameInFlightIndex;

    VkExtent2D fullExtent;
};
} // namespace PreVEngine

#endif
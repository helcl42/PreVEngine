#ifndef __RENDER_CONTEXTS_H__
#define __RENDER_CONTEXTS_H__

#include <prev/core/Core.h>
#include <prev/render/RenderContext.h>
#include <prev/util/intersection/Frustum.h>

namespace prev_test::render::renderer {
struct ShadowsRenderContext : prev::render::RenderContext {
    glm::mat4 viewMatrix;

    glm::mat4 projectionMatrix;

    uint32_t cascadeIndex;

    prev::util::intersection::Frustum frustum;

    ShadowsRenderContext(const RenderContext& ctx, const glm::mat4& vm, const glm::mat4& pm, const uint32_t index, const prev::util::intersection::Frustum& frstm)
        : RenderContext{ ctx }
        , viewMatrix{ vm }
        , projectionMatrix{ pm }
        , cascadeIndex{ index }
        , frustum{ frstm }
    {
    }
};

struct NormalRenderContext : prev::render::RenderContext {
    glm::vec4 clipPlane;

    uint32_t cameraCount;

    glm::vec2 nearFarClippingPlanes[MAX_VIEW_COUNT]{};

    glm::mat4 viewMatrices[MAX_VIEW_COUNT]{};

    glm::mat4 projectionMatrices[MAX_VIEW_COUNT]{};

    glm::vec3 cameraPositions[MAX_VIEW_COUNT]{};

    prev::util::intersection::Frustum frustums[MAX_VIEW_COUNT]{};

    NormalRenderContext(const RenderContext& ctx, const glm::vec4& cp, const uint32_t cc)
        : RenderContext{ ctx }
        , clipPlane{ cp }
        , cameraCount{ cc }
    {
    }
};

} // namespace prev_test::render::renderer

#endif // !__RENDER_CONTEXTS_H__

#ifndef __RENDER_CONTEXTS_H__
#define __RENDER_CONTEXTS_H__

#include "../../common/intersection/Frustum.h"

#include <prev/core/Core.h>
#include <prev/render/RenderContext.h>

namespace prev_test::render::renderer {
struct ShadowsRenderContext : prev::render::RenderContext {
    const glm::mat4 viewMatrix;

    const glm::mat4 projectionMatrix;

    const uint32_t cascadeIndex;

    const prev_test::common::intersection::Frustum frustum;

    ShadowsRenderContext(const RenderContext& ctx, const glm::mat4& vm, const glm::mat4& pm, const uint32_t index, const prev_test::common::intersection::Frustum& frst)
        : RenderContext{ ctx }
        , viewMatrix{ vm }
        , projectionMatrix{ pm }
        , cascadeIndex{ index }
        , frustum{ frst }
    {
    }
};

struct NormalRenderContext : prev::render::RenderContext {
    const glm::mat4 viewMatrix;

    const glm::mat4 projectionMatrix;

    const glm::vec3 cameraPosition;

    const glm::vec4 clipPlane;

    const float verticalFov;

    const glm::vec2 nearFarClippingPlane;

    const prev_test::common::intersection::Frustum frustum;

    NormalRenderContext(const RenderContext& ctx, const glm::mat4& vm, const glm::mat4& pm, const glm::vec3& camPos, const glm::vec4& cp, const float vertFov, const glm::vec2& nearFar, const prev_test::common::intersection::Frustum& frst)
        : RenderContext{ ctx }
        , viewMatrix{ vm }
        , projectionMatrix{ pm }
        , cameraPosition{ camPos }
        , clipPlane{ cp }
        , verticalFov{ vertFov }
        , nearFarClippingPlane{ nearFar }
        , frustum{ frst }
    {
    }
};

} // namespace prev_test::render::renderer

#endif // !__RENDER_CONTEXTS_H__

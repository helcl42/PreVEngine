#ifndef __RENDER_CONTEXT_USER_DATA_H__
#define __RENDER_CONTEXT_USER_DATA_H__

#include "../../common/intersection/Frustum.h"

#include <prev/common/Common.h>
#include <prev/core/instance/Validation.h>
#include <prev/render/DefaultRenderContextUserData.h>

namespace prev_test::render {
struct ShadowsRenderContextUserData : prev::render::DefaultRenderContextUserData {
    const glm::mat4 viewMatrix;

    const glm::mat4 projectionMatrix;

    const uint32_t cascadeIndex;

    const prev_test::common::intersection::Frustum frustum;

    ShadowsRenderContextUserData(const glm::mat4& vm, const glm::mat4& pm, const uint32_t index, const prev_test::common::intersection::Frustum& frst)
        : viewMatrix(vm)
        , projectionMatrix(pm)
        , cascadeIndex(index)
        , frustum(frst)
    {
    }
};

struct NormalRenderContextUserData : prev::render::DefaultRenderContextUserData {
    const glm::mat4 viewMatrix;

    const glm::mat4 projectionMatrix;

    const glm::vec3 cameraPosition;

    const glm::vec4 clipPlane;

    const glm::vec2 nearFarClippingPlane;

    const prev_test::common::intersection::Frustum frustum;

    NormalRenderContextUserData(const glm::mat4& vm, const glm::mat4& pm, const glm::vec3& camPos, const glm::vec4& cp, const glm::vec2& nearFar, const prev_test::common::intersection::Frustum& frst)
        : viewMatrix(vm)
        , projectionMatrix(pm)
        , cameraPosition(camPos)
        , clipPlane(cp)
        , nearFarClippingPlane(nearFar)
        , frustum(frst)
    {
    }
};

} // namespace prev_test::render

#endif // !__DEFAULT_RENDER_CONTEXT_USER_DATA_H__

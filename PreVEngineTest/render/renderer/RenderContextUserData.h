#ifndef __DEFAULT_RENDER_CONTEXT_USER_DATA_H__
#define __DEFAULT_RENDER_CONTEXT_USER_DATA_H__

#include "../../common/intersection/Frustum.h"

#include <prev/common/Common.h>
#include <prev/core/instance/Validation.h>

namespace prev_test::render {
struct DefaultRenderContextUserData // inherit this in case you need any special data while rendering scene graph
{
    virtual ~DefaultRenderContextUserData() = default;
};

struct ShadowsRenderContextUserData : DefaultRenderContextUserData {
    const glm::mat4 viewMatrix;

    const glm::mat4 projectionMatrix;

    const uint32_t cascadeIndex;

    const prev_test::common::intersection::Frustum frustum;

    const VkExtent2D extent;

    ShadowsRenderContextUserData(const glm::mat4& vm, const glm::mat4& pm, const uint32_t index, const prev_test::common::intersection::Frustum& frst, const VkExtent2D& ext)
        : viewMatrix(vm)
        , projectionMatrix(pm)
        , cascadeIndex(index)
        , frustum(frst)
        , extent(ext)
    {
    }
};

struct NormalRenderContextUserData : DefaultRenderContextUserData {
    const glm::mat4 viewMatrix;

    const glm::mat4 projectionMatrix;

    const glm::vec3 cameraPosition;

    const glm::vec4 clipPlane;

    const VkExtent2D extent;

    const glm::vec2 nearFarClippingPlane;

    const prev_test::common::intersection::Frustum frustum;

    NormalRenderContextUserData(const glm::mat4& vm, const glm::mat4& pm, const glm::vec3& camPos, const glm::vec4& cp, const VkExtent2D& ext, const glm::vec2& nearFar, const prev_test::common::intersection::Frustum& frst)
        : viewMatrix(vm)
        , projectionMatrix(pm)
        , cameraPosition(camPos)
        , clipPlane(cp)
        , extent(ext)
        , nearFarClippingPlane(nearFar)
        , frustum(frst)
    {
    }
};

} // namespace prev_test::render

#endif // !__DEFAULT_RENDER_CONTEXT_USER_DATA_H__

#ifndef __SANDBOX_CAMERA_COMPONENT_H__
#define __SANDBOX_CAMERA_COMPONENT_H__

#include "../common/Common.h"

#include <prev/scene/component/IComponent.h>

#include <glm/glm.hpp>

#include <cstdint>

namespace sandbox::component {
// Per-view camera state consumed by the renderer: the eye transform and a precomputed projection,
// one entry per view (a single view in mono, two for stereo XR). The camera scene nodes fill these
// in each frame (DefaultCamera from FOV + viewport aspect, XrCamera from the runtime's per-eye
// poses/FOVs), so the renderer just reads them — it never builds a projection itself.
class CameraComponent final : public prev::scene::component::IComponent {
public:
    explicit CameraComponent(uint32_t viewCount = 1)
        : m_viewCount{ viewCount }
    {
    }

    uint32_t GetViewCount() const
    {
        return m_viewCount;
    }

    void SetViewCount(uint32_t viewCount)
    {
        m_viewCount = viewCount;
    }

    const glm::vec3& GetPositions(uint32_t index) const
    {
        return m_positions[index];
    }

    void SetPositions(uint32_t index, const glm::vec3& position)
    {
        m_positions[index] = position;
    }

    const glm::mat4& GetViewMatrix(uint32_t index) const
    {
        return m_viewMatrices[index];
    }

    void SetViewMatrix(uint32_t index, const glm::mat4& viewMatrix)
    {
        m_viewMatrices[index] = viewMatrix;
    }

    const glm::mat4& GetProjectionMatrix(uint32_t index) const
    {
        return m_projectionMatrices[index];
    }

    void SetProjectionMatrix(uint32_t index, const glm::mat4& projectionMatrix)
    {
        m_projectionMatrices[index] = projectionMatrix;
    }

private:
    uint32_t m_viewCount{ 1 };

    glm::vec3 m_positions[MAX_VIEW_COUNT_VALUE]{}; // per-view eye position (world space)
    glm::mat4 m_viewMatrices[MAX_VIEW_COUNT_VALUE]{}; // per-view world->eye transform
    glm::mat4 m_projectionMatrices[MAX_VIEW_COUNT_VALUE]{}; // per-view eye->clip (Vulkan convention)
};
} // namespace sandbox::component

#endif // !__SANDBOX_CAMERA_COMPONENT_H__

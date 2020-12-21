#ifndef __RAY_CASTER_COMPONENT_H__
#define __RAY_CASTER_COMPONENT_H__

#include "IRayCasterComponent.h"

#include "../../common/intersection/Ray.h"
#include "../../render/IModel.h"

namespace prev_test::component::ray_casting {
class RayCasterComponent final : public IRayCasterComponent {
public:
    RayCasterComponent() = default;

    ~RayCasterComponent() = default;

public:
    void Update(float deltaTime) override;

    void SetRayDirection(const glm::vec3& rayDirection) override;

    void SetRayLength(const float len) override;

    void SetRayStartPosition(const glm::vec3& position) override;

    void SetOrientationOffsetAngles(const glm::vec2& angles) override;

    prev_test::common::intersection::Ray GetRay() const override;

#ifdef RENDER_RAYCASTS
    std::shared_ptr<prev_test::render::IModel> GetModel() const override;
#endif
private:
    glm::vec3 m_rayDirection;

    glm::vec3 m_rayStartPosition;

    float m_rayLength{ 500.0f };

    glm::vec2 m_orientationOffsetAngles{ 0.0f, 0.0f };
#ifdef RENDER_RAYCASTS
    std::shared_ptr<prev_test::render::IModel> m_model;
#endif
};
} // namespace prev_test::component::ray_casting

#endif // !__RAY_CASTER_COMPONENT_H__

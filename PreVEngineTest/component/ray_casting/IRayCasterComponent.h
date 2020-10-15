#ifndef __IRAY_CASTER_COMPONENT_H__
#define __IRAY_CASTER_COMPONENT_H__

#include "../../common/intersection/Ray.h"
#include "../../render/IModel.h"

namespace prev_test::component::ray_casting {
class IRayCasterComponent {
public:
    virtual void Update(float deltaTime) = 0;

    virtual void SetRayDirection(const glm::vec3& rayDirection) = 0;

    virtual void SetRayLength(const float len) = 0;

    virtual void SetRayStartPosition(const glm::vec3& position) = 0;

    virtual void SetOrientationOffsetAngles(const glm::vec2& angles) = 0;

    virtual prev_test::common::intersection::Ray GetRay() const = 0;

#ifdef RENDER_RAYCASTS
    virtual std::shared_ptr<prev_test::render::IModel> GetModel() const = 0;
#endif
public:
    virtual ~IRayCasterComponent() = default;
};
} // namespace prev_test::component::ray_casting

#endif // !__IRAY_CASTER_COMPONENT_H__

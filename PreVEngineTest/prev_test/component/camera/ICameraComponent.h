#ifndef __ICAMERA_COMPONENT_H__
#define __ICAMERA_COMPONENT_H__

#include "../../render/ViewFrustum.h"

namespace prev_test::component::camera {
class ICameraComponent {
public:
    virtual const glm::mat4& LookAt() const = 0;

    virtual void Reset() = 0;

    virtual void AddPitch(const float angle) = 0;

    virtual void AddYaw(const float angle) = 0;

    virtual void AddRoll(const float angle) = 0;

    virtual void AddOrientation(const float angle, const glm::vec3& axis) = 0;

    virtual void AddOrientation(const glm::quat& orientationDiff) = 0;

    virtual void SetOrientation(const glm::quat& orientation) = 0;

    virtual void AddPosition(const glm::vec3& positionDiff) = 0;

    virtual void SetPosition(const glm::vec3& position) = 0;

    virtual const glm::vec3& GetForwardDirection() const = 0;

    virtual const glm::vec3& GetRightDirection() const = 0;

    virtual const glm::vec3& GetUpDirection() const = 0;

    virtual const glm::vec3& GetDefaultUpDirection() const = 0;

    virtual const glm::vec3& GetPosition() const = 0;

    virtual const glm::quat& GetOrientation() const = 0;

    virtual const prev_test::render::ViewFrustum& GetViewFrustum() const = 0;

    virtual void SetViewFrustum(const prev_test::render::ViewFrustum& viewFrustum) = 0;

public:
    virtual ~ICameraComponent() = default;
};
} // namespace prev_test::component::camera

#endif // !__ICAMERA_COMPONENT_H__

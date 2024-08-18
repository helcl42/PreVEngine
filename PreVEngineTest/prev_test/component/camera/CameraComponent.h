#ifndef __SIX_DOF_CAMERA_COMPONENT_H__
#define __SIX_DOF_CAMERA_COMPONENT_H__

#include "ICameraComponent.h"

namespace prev_test::component::camera {
class CameraComponent : public ICameraComponent {
public:
    CameraComponent(const glm::quat& initialOrientation, const glm::vec3& initialPosition, const bool useFixedUp);

    virtual ~CameraComponent() = default;

public:
    const glm::mat4& LookAt() const override;

    void Reset() override;

    void AddPitch(const float angle) override;

    void AddYaw(const float angle) override;

    void AddRoll(const float angle) override;

    void AddOrientation(const float angle, const glm::vec3& axis) override;

    void AddOrientation(const glm::quat& orientationDiff) override;

    void SetOrientation(const glm::quat& orientation) override;

    void AddPosition(const glm::vec3& positionDiff) override;

    void SetPosition(const glm::vec3& position) override;

    const glm::vec3& GetForwardDirection() const override;

    const glm::vec3& GetRightDirection() const override;

    const glm::vec3& GetUpDirection() const override;

    const glm::vec3& GetDefaultUpDirection() const override;

    const glm::vec3& GetPosition() const override;

    const glm::quat& GetOrientation() const override;

    const prev_test::render::ViewFrustum& GetViewFrustum() const override;

    void SetViewFrustum(const prev_test::render::ViewFrustum& viewFrustum) override;

private:
    void UpdatePosition();

    void UpdateOrientation();

    void Update();

private:
    glm::quat m_initialOrientation;

    glm::vec3 m_initialPosition;

    bool m_useFixedUp;

private:
    static const inline glm::vec3 DEFAULT_UP_DIRECTION{ 0.0f, 1.0f, 0.0f };

    static const inline float NEAR_CLIPPING_PLANE{ 0.1f };

    static const inline float FAR_CLIPPING_PLANE{ 300.0f };

private:
    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };

    glm::quat m_orientation{ 1.0f, 0.0f, 0.0f, 0.0f };

    glm::vec3 m_rightDirection{ 0.0f, 0.0f, 0.0f };

    glm::vec3 m_upDirection{ 0.0f, 0.0f, 0.0f };

    glm::vec3 m_forwardDirection{ 0.0f, 0.0f, 0.0f };

    glm::vec3 m_positionDelta{ 0.0f, 0.0f, 0.0f };

    glm::quat m_orientationDelta{ 1.0f, 0.0f, 0.0f, 0.0f };

    glm::mat4 m_viewMatrix{ 1.0f };

    prev_test::render::ViewFrustum m_viewFrustum{ 45.0f, REVERSE_DEPTH ? FAR_CLIPPING_PLANE : NEAR_CLIPPING_PLANE, REVERSE_DEPTH ? NEAR_CLIPPING_PLANE : FAR_CLIPPING_PLANE };

    bool m_orientationChanged{ false };

    bool m_positionChanged{ false };
};
} // namespace prev_test::component::camera

#endif // !__SIX_DOF_CAMERA_COMPONENT_H__

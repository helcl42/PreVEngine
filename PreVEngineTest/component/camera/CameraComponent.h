#ifndef __CAMERA_COMPONENET_H__
#define __CAMERA_COMPONENET_H__

#include "ICameraComponent.h"

namespace prev_test::component::camera {
class CameraComponent : public ICameraComponent {
public:
    CameraComponent(const glm::quat initialOrientation, const glm::vec3& initialPosition);

    virtual ~CameraComponent() = default;

public:
    const glm::mat4& LookAt() const override;

    void Reset() override;

    void AddPitch(float amountInDegrees) override;

    void AddYaw(float amountInDegrees) override;

    void AddOrientation(const glm::quat& orientationDiff) override;

    void SetOrientation(const glm::quat& orientation) override;

    void SetOrientation(const float pitchAmountInDegrees, const float yawAmountInDeregrees) override;

    void AddPosition(const glm::vec3& positionDiff) override;

    void SetPosition(const glm::vec3& position) override;

    const glm::vec3& GetForwardDirection() const override;

    const glm::vec3& GetRightDirection() const override;

    const glm::vec3& GetUpDirection() const override;

    const glm::vec3& GetPosition() const override;

    const glm::quat& GetOrientation() const override;

    const prev_test::render::ViewFrustum& GetViewFrustum() const override;

    void SetViewFrustum(const prev_test::render::ViewFrustum& viewFrustum) override;

private:
    void UpdatePosition();

    void UpdateOrientation();

    void Update();

private:
    const glm::vec3 m_upDirection{ 0.0f, 1.0f, 0.0f };

    const glm::vec3 m_defaultForwardDirection{ 0.0f, 0.0f, -1.0f };

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialPosition;

private:
    glm::vec3 m_position;

    glm::quat m_orientation;

    glm::vec3 m_positionDelta;

    glm::vec3 m_forwardDirection;

    glm::vec3 m_rightDirection;

    glm::quat m_orientationDelta;

    glm::mat4 m_viewMatrix;

    glm::vec2 m_prevTouchPosition;

    prev_test::render::ViewFrustum m_viewFrustum{ 45.0f, 10.0f, 300.0f };

    bool m_orientationChanged{ false };

    bool m_positionChanged{ false };
};
} // namespace prev_test::component::camera

#endif // !__CAMERA_COMPONENET_H__

#include "Camera.h"
#include "../component/camera/CameraComponentFactory.h"
#include "../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/MathUtils.h>

namespace prev_test::scene {
Camera::Camera()
    : SceneNode()
{
}

void Camera::Init()
{
    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create();
    if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, TAG_TRANSFORM_COMPONENT);

    prev_test::component::camera::CameraComponentFactory cameraFactory{};
    m_cameraComponent = cameraFactory.Create(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 60.0f, 180.0f));
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::camera::ICameraComponent>(GetThis(), m_cameraComponent, TAG_TRANSFORM_COMPONENT);

    SceneNode::Init();

    Reset();
}

void Camera::Update(float deltaTime)
{
    glm::vec3 positionDelta{ 0.0f, 0.0f, 0.0f };
    if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_W)) {
        positionDelta += m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
    }
    if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_S)) {
        positionDelta -= m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
    }
    if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_A)) {
        positionDelta -= m_cameraComponent->GetRightDirection() * deltaTime * m_moveSpeed;
    }
    if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_D)) {
        positionDelta += m_cameraComponent->GetRightDirection() * deltaTime * m_moveSpeed;
    }
    if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_Q)) {
        positionDelta -= m_cameraComponent->GetUpDirection() * deltaTime * m_moveSpeed;
    }
    if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_E)) {
        positionDelta += m_cameraComponent->GetUpDirection() * deltaTime * m_moveSpeed;
    }

#if defined(__ANDROID__)
    if (m_autoMoveForward) {
        positionDelta += m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
    }

    if (m_autoMoveBackward) {
        positionDelta -= m_cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
    }
#endif

    m_cameraComponent->AddPosition(positionDelta);

    glm::mat4 viewMatrix = m_cameraComponent->LookAt();
    glm::mat4 cameraTransformInWorldSpace = glm::inverse(viewMatrix);

    m_transformComponent->SetPosition(prev::util::MathUtil::ExtractTranslation(cameraTransformInWorldSpace));
    m_transformComponent->SetOrientation(prev::util::MathUtil::ExtractRotationAsQuaternion(cameraTransformInWorldSpace));

    m_transformComponent->Update(deltaTime);

    SceneNode::Update(deltaTime);
}

void Camera::ShutDown()
{
    SceneNode::ShutDown();
}

void Camera::operator()(const prev::input::mouse::MouseEvent& mouseEvent)
{
    if (mouseEvent.action == prev::input::mouse::MouseActionType::MOVE && mouseEvent.button == prev::input::mouse::MouseButtonType::LEFT) {
        const glm::vec2 angleInDegrees = mouseEvent.position * m_sensitivity;

        const float newPitch = m_pitchAngle + angleInDegrees.y;
        if (newPitch > -m_absMinMaxPitch && newPitch < m_absMinMaxPitch) {
            m_cameraComponent->AddPitch(angleInDegrees.y);
            m_pitchAngle += angleInDegrees.y;
        }
        m_cameraComponent->AddYaw(angleInDegrees.x);
    }
}

void Camera::operator()(const prev::input::touch::TouchEvent& touchEvent)
{
#if defined(__ANDROID__)
    const float MAX_RATIO_FOR_MOVE_CONTROL = 0.25; //
    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE || touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        const auto MAX_X_COORD_TO_CONTROL = touchEvent.extent.x * MAX_RATIO_FOR_MOVE_CONTROL;
        const auto MAX_Y_COORD_TO_BACKWARD_CONTROL = touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
        const auto MIN_Y_COORD_TO_BACKWARD_CONTROL = touchEvent.extent.y - touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
        if (touchEvent.position.x < MAX_X_COORD_TO_CONTROL && touchEvent.position.y < MAX_Y_COORD_TO_BACKWARD_CONTROL) {
            m_autoMoveForward = true;
        }

        if (touchEvent.position.x < MAX_X_COORD_TO_CONTROL && touchEvent.position.y > MIN_Y_COORD_TO_BACKWARD_CONTROL) {
            m_autoMoveBackward = true;
        }
    } else {
        m_autoMoveForward = false;
        m_autoMoveBackward = false;
        return;
    }
#endif
    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE) {
        const glm::vec2 angleInDegrees = (touchEvent.position - m_prevTouchPosition) * m_sensitivity;

        m_cameraComponent->AddPitch(angleInDegrees.y);
        m_cameraComponent->AddYaw(angleInDegrees.x);
    }

    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE || touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        m_prevTouchPosition = touchEvent.position;
    }
}

void Camera::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS) {
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_R) {
            Reset();
        }
    }
}

void Camera::Reset()
{
    m_cameraComponent->Reset();

    m_prevTouchPosition = glm::vec2(0.0f, 0.0f);
#if defined(__ANDROID__)
    m_autoMoveForward = false;
    m_autoMoveBackward = false;
#endif
}
} // namespace prev_test::scene
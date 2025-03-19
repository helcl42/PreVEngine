#include "Camera.h"

#include "../component/camera/CameraComponentFactory.h"
#include "../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/MathUtils.h>

namespace prev_test::scene {
Camera::Camera(uint32_t viewCount)
    : SceneNode()
    , m_viewCount{ viewCount }
{
}

void Camera::Init()
{
    for (uint32_t view = 0; view < m_viewCount; ++view) {
        std::shared_ptr<prev_test::component::transform::ITransformComponent> transformComponent{ prev_test::component::transform::TrasnformComponentFactory{}.Create() };
        if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
            transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
        }
        prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), transformComponent, TAG_TRANSFORM_COMPONENT);

        std::shared_ptr<prev_test::component::camera::ICameraComponent> cameraComponent{ prev_test::component::camera::CameraComponentFactory{}.Create(glm::quat{}, glm::vec3{}, false) };
        prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::camera::ICameraComponent>(GetThis(), cameraComponent, TAG_TRANSFORM_COMPONENT);

        m_transformComponents.push_back(transformComponent);
        m_cameraComponents.push_back(cameraComponent);
    }

    SceneNode::Init();

    Reset();
}

void Camera::Update(float deltaTime)
{
    for (uint32_t view = 0; view < m_viewCount; ++view) {
        auto transformComponent{ m_transformComponents[view] };
        auto cameraComponent{ m_cameraComponents[view] };

#ifndef ENABLE_XR
        glm::vec3 positionDelta{ 0.0f, 0.0f, 0.0f };
        if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_W)) {
            positionDelta += cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_S)) {
            positionDelta -= cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_A)) {
            positionDelta -= cameraComponent->GetRightDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_D)) {
            positionDelta += cameraComponent->GetRightDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_Q)) {
            positionDelta -= cameraComponent->GetUpDirection() * deltaTime * m_moveSpeed;
        }
        if (m_inputFacade.IsKeyPressed(prev::input::keyboard::KeyCode::KEY_E)) {
            positionDelta += cameraComponent->GetUpDirection() * deltaTime * m_moveSpeed;
        }

#if defined(__ANDROID__)
        if (m_autoMoveForward) {
            positionDelta += cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }

        if (m_autoMoveBackward) {
            positionDelta -= cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }
#endif

        cameraComponent->AddPosition(positionDelta);
        cameraComponent->SetViewFrustum(prev_test::render::ViewFrustum{ cameraComponent->GetViewFrustum().GetVerticalFov(), static_cast<float>(m_viewPortSize.x) / static_cast<float>(m_viewPortSize.y), cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane() });
#endif

        transformComponent->SetPosition(cameraComponent->GetPosition());
        transformComponent->SetOrientation(cameraComponent->GetOrientation());

        transformComponent->Update(deltaTime);
    }

    SceneNode::Update(deltaTime);
}

void Camera::ShutDown()
{
    SceneNode::ShutDown();
}

void Camera::operator()(const prev::input::mouse::MouseEvent& mouseEvent)
{
    if (mouseEvent.action == prev::input::mouse::MouseActionType::MOVE && mouseEvent.button == prev::input::mouse::MouseButtonType::LEFT) {
        const glm::vec2 angleInDegrees{ mouseEvent.position * m_sensitivity };
        for (auto& cameraComponent : m_cameraComponents) {
            cameraComponent->AddPitch(glm::radians(-angleInDegrees.y));
            cameraComponent->AddYaw(glm::radians(angleInDegrees.x));
        }
    }
}

void Camera::operator()(const prev::input::touch::TouchEvent& touchEvent)
{
#if defined(__ANDROID__)
    const float MAX_RATIO_FOR_MOVE_CONTROL{ 0.25 };
    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE || touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        const auto MAX_X_COORD_TO_CONTROL{ touchEvent.extent.x * MAX_RATIO_FOR_MOVE_CONTROL };
        const auto MAX_Y_COORD_TO_BACKWARD_CONTROL{ touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL };
        const auto MIN_Y_COORD_TO_BACKWARD_CONTROL{ touchEvent.extent.y - touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL };
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
        const glm::vec2 angleInDegrees{ (touchEvent.position - m_prevTouchPosition) * m_sensitivity };
        for (auto& cameraComponent : m_cameraComponents) {
            cameraComponent->AddPitch(angleInDegrees.y);
            cameraComponent->AddYaw(angleInDegrees.x);
        }
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

#ifdef ENABLE_XR
void Camera::operator()(const prev::xr::XrCameraEvent& cameraEvent)
{
    for (uint32_t view = 0; view < cameraEvent.count; ++view) {
        auto& cameraComponent{ m_cameraComponents[view] };
        const auto previousFrustum{ cameraComponent->GetViewFrustum() };

        const auto& fov{ cameraEvent.fovs[view] };
        prev_test::render::ViewFrustum viewFrustum{ fov.angleLeft, fov.angleRight, fov.angleUp, fov.angleDown, previousFrustum.GetNearClippingPlane(), previousFrustum.GetFarClippingPlane() };
        cameraComponent->SetViewFrustum(viewFrustum);

        const auto& pose{ cameraEvent.poses[view] };
        cameraComponent->SetOrientation(pose.orientation);
        cameraComponent->SetPosition(pose.position);
    }

    const auto viewFrustum{ m_cameraComponents[0]->GetViewFrustum() };
    prev::event::EventChannel::Post(prev::xr::XrCameraFeedbackEvent{ viewFrustum.GetNearClippingPlane(), viewFrustum.GetFarClippingPlane(), MIN_DEPTH, MAX_DEPTH });
}
#else
void Camera::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
}
#endif

void Camera::Reset()
{
    for (auto& cameraComponent : m_cameraComponents) {
        cameraComponent->Reset();
    }

    m_prevTouchPosition = glm::vec2(0.0f, 0.0f);
#if defined(__ANDROID__)
    m_autoMoveForward = false;
    m_autoMoveBackward = false;
#endif
}
} // namespace prev_test::scene
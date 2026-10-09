#include "Camera.h"

#include "XrEyes.h"

#include "../Tags.h"
#include "../component/camera/CameraComponentFactory.h"
#include "../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/MathUtils.h>

#include <algorithm>

namespace prev_test::scene {
Camera::Camera(uint32_t viewCount, const glm::vec3& position, const glm::quat& orientation)
    : SceneNode({ TAG_MAIN_CAMERA, TAG_PLAYER })
    , m_viewCount{ viewCount }
    , m_position{ position }
    , m_orientation{ orientation }
{
}

void Camera::Init()
{
    for (uint32_t view = 0; view < m_viewCount; ++view) {
        std::shared_ptr<prev_test::component::transform::ITransformComponent> transformComponent{ prev_test::component::transform::TrasnformComponentFactory{}.Create() };
        if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
            transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
        }
        prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), transformComponent, { TAG_TRANSFORM_COMPONENT });

        std::shared_ptr<prev_test::component::camera::ICameraComponent> cameraComponent{ prev_test::component::camera::CameraComponentFactory{}.Create(m_orientation, m_position, true) };
        prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::camera::ICameraComponent>(GetThis(), cameraComponent, { TAG_CAMERA_COMPONENT });

        m_transformComponents.push_back(transformComponent);
        m_cameraComponents.push_back(cameraComponent);
    }
#ifdef ENABLE_XR
    AddChild(std::make_shared<XrEyes>(m_cameraComponents)); // while an immersive session runs, the headset places the eyes
#endif

    SceneNode::Init();

    Reset();
}

void Camera::Update(float deltaTime)
{
    for (uint32_t view = 0; view < m_viewCount; ++view) {
        auto transformComponent{ m_transformComponents[view] };
        auto cameraComponent{ m_cameraComponents[view] };

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

        if (m_autoMoveForward) {
            positionDelta += cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }

        if (m_autoMoveBackward) {
            positionDelta -= cameraComponent->GetForwardDirection() * deltaTime * m_moveSpeed;
        }

        cameraComponent->AddPosition(positionDelta);
        cameraComponent->SetViewFrustum(prev_test::render::ViewFrustum{ cameraComponent->GetViewFrustum().GetVerticalFov(), static_cast<float>(m_viewPortSize.x) / static_cast<float>(m_viewPortSize.y), cameraComponent->GetViewFrustum().GetNearClippingPlane(), cameraComponent->GetViewFrustum().GetFarClippingPlane() });
    }

    SceneNode::Update(deltaTime);

    // Update the transform components as post step
    for (uint32_t view = 0; view < m_viewCount; ++view) {
        auto transformComponent{ m_transformComponents[view] };
        auto cameraComponent{ m_cameraComponents[view] };

        transformComponent->SetPosition(cameraComponent->GetPosition());
        transformComponent->SetOrientation(cameraComponent->GetOrientation());

        transformComponent->Update(deltaTime);
    }
}

void Camera::ShutDown()
{
    SceneNode::ShutDown();
}

void Camera::operator()(const prev::input::mouse::MouseEvent& mouseEvent)
{
    if (mouseEvent.action == prev::input::mouse::MouseActionType::MOVE && mouseEvent.button == prev::input::mouse::MouseButtonType::LEFT) {
        AddLook(mouseEvent.position * m_sensitivity);
    }
}

void Camera::operator()(const prev::input::touch::TouchEvent& touchEvent)
{
    // Screen corner/edge regions (quarter of each side), mirroring Player's layout.
    const float regionRatio{ 0.25f };
    const glm::vec2 maxPoint{ touchEvent.extent * regionRatio }; // near top-left
    const glm::vec2 minPoint{ touchEvent.extent - maxPoint };    // near bottom-right

    // Tap the top-left corner to reset (touch equivalent of the R key).
    if (touchEvent.action == prev::input::touch::TouchActionType::DOWN && touchEvent.position.x < maxPoint.x && touchEvent.position.y < maxPoint.y) {
        Reset();
    }

    // Right-edge move pad (mirrors Player): top-right moves forward, bottom-right moves backward.
    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE || touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        m_autoMoveForward = touchEvent.position.x > minPoint.x && touchEvent.position.y < maxPoint.y;
        m_autoMoveBackward = touchEvent.position.x > minPoint.x && touchEvent.position.y > minPoint.y;
    } else {
        m_autoMoveForward = false;
        m_autoMoveBackward = false;
    }

    // Drag anywhere to look around.
    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE) {
        AddLook((touchEvent.position - m_prevTouchPosition) * m_sensitivity);
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

void Camera::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
}

void Camera::AddLook(const glm::vec2& deltaDegrees)
{
    for (auto& cameraComponent : m_cameraComponents) {
        cameraComponent->AddPitch(glm::radians(-deltaDegrees.y));
        cameraComponent->AddYaw(glm::radians(deltaDegrees.x));
    }
}

void Camera::Reset()
{
    for (auto& cameraComponent : m_cameraComponents) {
        cameraComponent->Reset();
    }

    m_prevTouchPosition = glm::vec2(0.0f, 0.0f);
    m_autoMoveForward = false;
    m_autoMoveBackward = false;
}
} // namespace prev_test::scene
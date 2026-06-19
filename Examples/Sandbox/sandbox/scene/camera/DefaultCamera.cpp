#include "DefaultCamera.h"

#include <prev/util/MathUtils.h>

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace sandbox::scene::camera {
DefaultCamera::DefaultCamera(uint32_t viewCount)
    : Camera{ viewCount }
{
}

void DefaultCamera::Init()
{
    Camera::Init(); // creates and registers the CameraComponent
    UpdateView(); // seed view 0 so the very first frame already has a valid transform
}

void DefaultCamera::Update(float deltaTime)
{
    // Free-fly: WASD moves in the view plane, Q/E moves vertically (world up).
    const float distance{ m_moveSpeed * deltaTime };
    const glm::vec3 forward{ GetForwardDirection() };
    const glm::vec3 right{ GetRightDirection() };
    const glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };

    using prev::input::keyboard::KeyCode;
    if (m_inputFacade.IsKeyPressed(KeyCode::KEY_W)) {
        m_position += forward * distance;
    }
    if (m_inputFacade.IsKeyPressed(KeyCode::KEY_S)) {
        m_position -= forward * distance;
    }
    if (m_inputFacade.IsKeyPressed(KeyCode::KEY_A)) {
        m_position -= right * distance;
    }
    if (m_inputFacade.IsKeyPressed(KeyCode::KEY_D)) {
        m_position += right * distance;
    }
    if (m_inputFacade.IsKeyPressed(KeyCode::KEY_Q)) {
        m_position -= worldUp * distance;
    }
    if (m_inputFacade.IsKeyPressed(KeyCode::KEY_E)) {
        m_position += worldUp * distance;
    }

    // Touch move pad (forward/backward only).
    if (m_touchMoveForward) {
        m_position += forward * distance;
    }
    if (m_touchMoveBackward) {
        m_position -= forward * distance;
    }

    UpdateView();

    SceneNode::Update(deltaTime);
}

void DefaultCamera::operator()(const prev::input::mouse::MouseEvent& mouseEvent)
{
    using prev::input::mouse::MouseActionType;
    using prev::input::mouse::MouseButtonType;

    // Look while dragging with the left button. MOVE reports no button and an absolute position,
    // so we latch the drag on PRESS/RELEASE and rotate by the per-move position delta.
    if (mouseEvent.action == MouseActionType::PRESS && mouseEvent.button == MouseButtonType::LEFT) {
        m_dragging = true;
        m_prevMousePosition = mouseEvent.position;
    } else if (mouseEvent.action == MouseActionType::RELEASE && mouseEvent.button == MouseButtonType::LEFT) {
        m_dragging = false;
    } else if (mouseEvent.action == MouseActionType::MOVE && m_dragging) {
        const glm::vec2 deltaDegrees{ (mouseEvent.position - m_prevMousePosition) * m_sensitivity };
        AddLook(glm::vec2{ deltaDegrees.x, -deltaDegrees.y });
        m_prevMousePosition = mouseEvent.position;
    }
}

void DefaultCamera::operator()(const prev::input::touch::TouchEvent& touchEvent)
{
    using prev::input::touch::TouchActionType;

    // Screen corner/edge regions (quarter of each side), mirroring Player's layout.
    const float regionRatio{ 0.25f };
    const glm::vec2 maxPoint{ touchEvent.extent * regionRatio };          // near top-left
    const glm::vec2 minPoint{ touchEvent.extent - maxPoint };            // near bottom-right

    // Tap the top-left corner to reset (touch equivalent of the R key).
    if (touchEvent.action == TouchActionType::DOWN && touchEvent.position.x < maxPoint.x && touchEvent.position.y < maxPoint.y) {
        Reset();
    }

    // Right-edge move pad (mirrors Player): top-right moves forward, bottom-right moves backward.
    if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
        m_touchMoveForward = touchEvent.position.x > minPoint.x && touchEvent.position.y < maxPoint.y;
        m_touchMoveBackward = touchEvent.position.x > minPoint.x && touchEvent.position.y > minPoint.y;
    } else {
        m_touchMoveForward = false;
        m_touchMoveBackward = false;
    }

    // Drag anywhere to look around.
    if (touchEvent.action == TouchActionType::MOVE) {
        const glm::vec2 deltaDegrees{ (touchEvent.position - m_prevTouchPosition) * m_sensitivity };
        AddLook(deltaDegrees);
    }
    if (touchEvent.action == TouchActionType::MOVE || touchEvent.action == TouchActionType::DOWN) {
        m_prevTouchPosition = touchEvent.position;
    }
}

void DefaultCamera::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS && keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_R) {
        Reset();
    }
}

void DefaultCamera::Reset()
{
    m_position = INITIAL_POSITION;
    m_yaw = 0.0f;
    m_pitch = INITIAL_PITCH;
    m_dragging = false;
    m_prevMousePosition = glm::vec2{ 0.0f };
    m_prevTouchPosition = glm::vec2{ 0.0f };
    m_touchMoveForward = false;
    m_touchMoveBackward = false;
    UpdateView();
}

void DefaultCamera::AddLook(const glm::vec2& deltaDegrees)
{
    m_yaw += glm::radians(deltaDegrees.x);
    m_pitch += glm::radians(deltaDegrees.y);
    m_pitch = glm::clamp(m_pitch, glm::radians(-89.0f), glm::radians(89.0f));
}

glm::vec3 DefaultCamera::GetForwardDirection() const
{
    return glm::normalize(glm::vec3{
        std::sin(m_yaw) * std::cos(m_pitch),
        std::sin(m_pitch),
        -std::cos(m_yaw) * std::cos(m_pitch) });
}

glm::vec3 DefaultCamera::GetRightDirection() const
{
    return glm::normalize(glm::cross(GetForwardDirection(), glm::vec3{ 0.0f, 1.0f, 0.0f }));
}

void DefaultCamera::operator()(const prev::core::NewIterationEvent& iterationEvent)
{
    if (iterationEvent.windowHeight > 0) {
        m_aspect = static_cast<float>(iterationEvent.windowWidth) / static_cast<float>(iterationEvent.windowHeight);
    }
}

void DefaultCamera::UpdateView()
{
    m_camera->SetViewCount(m_viewCount);
    m_camera->SetPositions(0, m_position);
    m_camera->SetViewMatrix(0, glm::lookAt(m_position, m_position + GetForwardDirection(), glm::vec3{ 0.0f, 1.0f, 0.0f }));
    m_camera->SetProjectionMatrix(0, prev::util::math::CreatePerspectiveProjectionMatrix(m_verticalFov, m_aspect, NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE));
}
} // namespace sandbox::scene::camera

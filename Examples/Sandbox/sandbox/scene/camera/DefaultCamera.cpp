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
    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE) {
        const glm::vec2 deltaDegrees{ (touchEvent.position - m_prevTouchPosition) * m_sensitivity };
        AddLook(deltaDegrees);
    }
    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE || touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
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

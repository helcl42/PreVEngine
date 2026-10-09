#ifndef __SANDBOX_CAMERA_H__
#define __SANDBOX_CAMERA_H__

#include "../../component/CameraComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/event/EventHandler.h>
#include <prev/input/InputFacade.h>
#include <prev/scene/graph/SceneNode.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <memory>

namespace sandbox::scene::camera {
class Camera final : public prev::scene::graph::SceneNode {
public:
    explicit Camera(uint32_t viewCount);

    ~Camera() override = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void operator()(const prev::input::mouse::MouseEvent& mouseEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

    void operator()(const prev::core::NewIterationEvent& iterationEvent);

public:
    static constexpr float NEAR_CLIPPING_PLANE{ 0.1f };
    static constexpr float FAR_CLIPPING_PLANE{ 100.0f };

private:
    void Reset();

    void UpdateView();

    glm::vec3 GetForwardDirection() const;

    glm::vec3 GetRightDirection() const;

    void AddLook(const glm::vec2& deltaDegrees);

private:
    static constexpr glm::vec3 INITIAL_POSITION{ 0.0f, 4.0f, 12.0f };
    static constexpr float INITIAL_PITCH{ -0.3f }; // radians, look slightly down at the grid

    const float m_sensitivity{ 0.1f };
    const float m_moveSpeed{ 8.0f };
    const float m_verticalFov{ glm::radians(60.0f) };

    uint32_t m_viewCount{ 1 };
    std::shared_ptr<sandbox::component::CameraComponent> m_camera;

    glm::vec3 m_position{ INITIAL_POSITION };
    float m_yaw{ 0.0f };            // radians, around +Y (0 looks toward -Z)
    float m_pitch{ INITIAL_PITCH }; // radians, around +X (clamped to avoid gimbal flip)
    float m_aspect{ 1280.0f / 720.0f }; // updated from the viewport each frame (NewIterationEvent)

    // MOVE events carry the absolute cursor position and report no button, so we track the
    // left-button drag ourselves and derive the look delta from successive positions.
    bool m_dragging{ false };
    glm::vec2 m_prevMousePosition{ 0.0f };
    glm::vec2 m_prevTouchPosition{ 0.0f };

    // Touch move pad (mirrors Player): touching the right edge drives forward/backward movement,
    // consumed in Update; drag anywhere still looks around.
    bool m_touchMoveForward{ false };
    bool m_touchMoveBackward{ false };

    prev::input::InputsFacade m_inputFacade;
    prev::event::EventHandler<Camera, prev::input::mouse::MouseEvent> m_mouseHandler{ *this };
    prev::event::EventHandler<Camera, prev::input::touch::TouchEvent> m_touchHandler{ *this };
    prev::event::EventHandler<Camera, prev::input::keyboard::KeyEvent> m_keyHandler{ *this };
    prev::event::EventHandler<Camera, prev::core::NewIterationEvent> m_iterationHandler{ *this };
};
} // namespace sandbox::scene::camera

#endif // !__SANDBOX_CAMERA_H__

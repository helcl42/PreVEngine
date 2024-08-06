#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../General.h"
#include "../component/camera/ICameraComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/input/InputFacade.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Camera final : public prev::scene::graph::SceneNode {
public:
    Camera();

    virtual ~Camera() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::mouse::MouseEvent& mouseEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

private:
    void Reset();

private:
    prev::event::EventHandler<Camera, prev::input::mouse::MouseEvent> m_mouseHandler{ *this };

    prev::event::EventHandler<Camera, prev::input::touch::TouchEvent> m_touchHandler{ *this };

    prev::event::EventHandler<Camera, prev::input::keyboard::KeyEvent> m_keyHandler{ *this };

private:
    const float m_sensitivity{ 0.03f };

    const float m_moveSpeed{ 25.0f };

    const float m_absMinMaxPitch{ 89.0f };

    float m_pitchAngle{ 0.0f };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

private:
    prev::input::InputsFacade m_inputFacade;

    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::camera::ICameraComponent> m_cameraComponent;

#if defined(__ANDROID__)
    bool m_autoMoveForward{ false };

    bool m_autoMoveBackward{ false };
#endif
};
} // namespace prev_test::scene

#endif // !__CAMERA_H__

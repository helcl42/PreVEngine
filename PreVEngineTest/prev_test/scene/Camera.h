#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../component/camera/ICameraComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/input/InputFacade.h>
#include <prev/scene/graph/SceneNode.h>

#ifdef ENABLE_XR
#include <prev/xr/XrEvents.h>
#endif

#include <vector>

namespace prev_test::scene {
class Camera final : public prev::scene::graph::SceneNode {
public:
    Camera(uint32_t viewCount);

    ~Camera() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::mouse::MouseEvent& mouseEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

#ifdef ENABLE_XR
    void operator()(const prev::xr::XrCameraEvent& cameraEvent);
#else
    void operator()(const prev::core::NewIterationEvent& newIterationEvent);
#endif

private:
    void Reset();

private:
    prev::event::EventHandler<Camera, prev::input::mouse::MouseEvent> m_mouseHandler{ *this };

    prev::event::EventHandler<Camera, prev::input::touch::TouchEvent> m_touchHandler{ *this };

    prev::event::EventHandler<Camera, prev::input::keyboard::KeyEvent> m_keyHandler{ *this };

#ifdef ENABLE_XR
    prev::event::EventHandler<Camera, prev::xr::XrCameraEvent> m_xrCameraEventHandler{ *this };
#else
    prev::event::EventHandler<Camera, prev::core::NewIterationEvent> m_newIterationEventHandler{ *this };
#endif

private:
    uint32_t m_viewCount{ 1 };

private:
    const float m_sensitivity{ 0.03f };

    const float m_moveSpeed{ 25.0f };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

private:
    prev::input::InputsFacade m_inputFacade;

    std::vector<std::shared_ptr<prev_test::component::transform::ITransformComponent>> m_transformComponents;

    std::vector<std::shared_ptr<prev_test::component::camera::ICameraComponent>> m_cameraComponents;

#if defined(__ANDROID__)
    bool m_autoMoveForward{ false };

    bool m_autoMoveBackward{ false };
#endif

#ifndef ENABLE_XR
    glm::uvec2 m_viewPortSize{ 1920, 1080 };
#endif
};
} // namespace prev_test::scene

#endif // !__CAMERA_H__

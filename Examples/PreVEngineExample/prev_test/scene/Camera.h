#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "../component/camera/ICameraComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/input/InputFacade.h>
#include <prev/scene/graph/SceneNode.h>

#include <vector>

namespace prev_test::scene {
class Camera final : public prev::scene::graph::SceneNode {
public:
    Camera(uint32_t viewCount, const glm::vec3& position, const glm::quat& orientation);

    ~Camera() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::mouse::MouseEvent& mouseEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

    void operator()(const prev::core::NewIterationEvent& newIterationEvent);

private:
    void Reset();

    void AddLook(const glm::vec2& deltaDegrees);

private:
    prev::event::EventHandler<Camera, prev::input::mouse::MouseEvent> m_mouseHandler{ *this };

    prev::event::EventHandler<Camera, prev::input::touch::TouchEvent> m_touchHandler{ *this };

    prev::event::EventHandler<Camera, prev::input::keyboard::KeyEvent> m_keyHandler{ *this };

    prev::event::EventHandler<Camera, prev::core::NewIterationEvent> m_newIterationEventHandler{ *this };

private:
    uint32_t m_viewCount{ 1 };

    glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };

    glm::quat m_orientation{ 1.0f, 0.0f, 0.0f, 0.0f };

private:
    const float m_sensitivity{ 0.03f };

    const float m_moveSpeed{ 25.0f };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

private:
    prev::input::InputsFacade m_inputFacade;

    std::vector<std::shared_ptr<prev_test::component::transform::ITransformComponent>> m_transformComponents;

    std::vector<std::shared_ptr<prev_test::component::camera::ICameraComponent>> m_cameraComponents;

    bool m_autoMoveForward{ false };

    bool m_autoMoveBackward{ false };

    glm::uvec2 m_viewPortSize{ 1920, 1080 };
};
} // namespace prev_test::scene

#endif // !__CAMERA_H__

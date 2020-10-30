#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "../General.h"
#include "../component/camera/ICameraComponent.h"
#include "../component/ray_casting/IBoundingVolumeComponent.h"
#include "../component/render/IAnimationRenderComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/input/mouse/MouseEvents.h>
#include <prev/input/touch/TouchEvents.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Player final : public prev::scene::graph::SceneNode {
public:
    Player(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);

    virtual ~Player() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

    void operator()(const prev::input::mouse::MouseEvent& mouseEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

    void operator()(const prev::input::mouse::MouseScrollEvent& scrollEvent);

private:
    const float RUN_SPEED{ 14.0f };

    const float YAW_TURN_SPEED{ 3.0f };

    const float PITCH_TURN_SPEED{ 0.5f };

    const float GRAVITY_Y{ -5.0f };

    const float JUMP_POWER{ 2.5f };

    const float MIN_Y_POS{ 5.5f };

private:
    const glm::vec3 m_initialPosition;

    const glm::quat m_initialOrientation;

    const glm::vec3 m_initialScale;

    bool m_shouldGoForward{ false };

    bool m_shouldGoBackward{ false };

    bool m_shouldGoLeft{ false };

    bool m_shouldGoRight{ false };

    bool m_shouldRotate{ false };

    float m_upwardSpeed{ 0.0f };

    float m_rotationAroundY{ 0.0f };

    float m_pitchDiff{ 0.0f };

    bool m_isInTheAir{ false };

    float m_cameraPitch{ -20.0f };

    float m_distanceFromPerson{ 50.0f };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

private:
    prev::event::EventHandler<Player, prev::input::keyboard::KeyEvent> m_keyboardEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::mouse::MouseEvent> m_mouseEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::touch::TouchEvent> m_touchEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::mouse::MouseScrollEvent> m_mouseScrollsHandler{ *this };

private:
    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::render::IAnimationRenderComponent> m_animatonRenderComponent;

    std::shared_ptr<prev_test::component::camera::ICameraComponent> m_cameraComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__PLAYER_H__

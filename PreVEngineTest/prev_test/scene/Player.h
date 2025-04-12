#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "../component/camera/ICameraComponent.h"
#include "../component/ray_casting/IBoundingVolumeComponent.h"
#include "../component/render/IAnimationRenderComponent.h"
#include "../component/transform/ITransformComponent.h"

#include <prev/core/CoreEvents.h>
#include <prev/core/device/Device.h>
#include <prev/core/memory/Allocator.h>
#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/input/mouse/MouseEvents.h>
#include <prev/input/touch/TouchEvents.h>
#include <prev/scene/graph/SceneNode.h>

namespace prev_test::scene {
class Player final : public prev::scene::graph::SceneNode {
public:
    Player(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale);

    ~Player() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::core::NewIterationEvent& newIterationEvent);

    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

    void operator()(const prev::input::mouse::MouseEvent& mouseEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

    void operator()(const prev::input::mouse::MouseScrollEvent& scrollEvent);

private:
    static const inline float RUN_SPEED{ 14.0f };

    static const inline float YAW_TURN_SPEED{ 1.0f };

    static const inline float PITCH_TURN_SPEED{ 0.15f };

    static const inline float GRAVITY_Y{ -10.0f };

    static const inline float JUMP_POWER{ 4.0f };

    static const inline uint32_t WALKING_ANIMATION_INDEX{ 0 };

    static const inline uint32_t JUMP_ANIMATION_INDEX{ 1 };

private:
    prev::core::device::Device& m_device;

    prev::core::memory::Allocator& m_allocator;

    glm::vec3 m_initialPosition;

    glm::quat m_initialOrientation;

    glm::vec3 m_initialScale;

    bool m_shouldGoForward{ false };

    bool m_shouldGoBackward{ false };

    bool m_shouldGoLeft{ false };

    bool m_shouldGoRight{ false };

    bool m_shouldRotate{ false };

    float m_upwardSpeed{ 0.0f };

    glm::vec3 m_pitchYawRollDiff{ 0.0f };

    bool m_isInTheAir{ false };

    float m_cameraPitch{ 20.0f };

    float m_cameraDistanceFromPerson{ 30.0f };

    float m_cameraPositionOffset{ 8.0 };

    glm::vec2 m_prevTouchPosition{ 0.0f, 0.0f };

    glm::ivec2 m_currentResolution{ 1920, 1080 };

private:
    prev::event::EventHandler<Player, prev::core::NewIterationEvent> m_newIterationEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::keyboard::KeyEvent> m_keyboardEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::mouse::MouseEvent> m_mouseEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::touch::TouchEvent> m_touchEventsHandler{ *this };

    prev::event::EventHandler<Player, prev::input::mouse::MouseScrollEvent> m_mouseScrollsHandler{ *this };

private:
    std::shared_ptr<prev_test::component::transform::ITransformComponent> m_transformComponent;

    std::shared_ptr<prev_test::component::render::IAnimationRenderComponent> m_animationRenderComponent;

    std::shared_ptr<prev_test::component::camera::ICameraComponent> m_cameraComponent;

    std::shared_ptr<prev_test::component::ray_casting::IBoundingVolumeComponent> m_boundingVolumeComponent;
};
} // namespace prev_test::scene

#endif // !__PLAYER_H__

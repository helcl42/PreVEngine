#ifndef __CUBE_ROBOT_H__
#define __CUBE_ROBOT_H__

#include "CubeRobotPart.h"

#include <prev/event/EventHandler.h>
#include <prev/input/keyboard/KeyboardEvents.h>
#include <prev/input/mouse/MouseEvents.h>
#include <prev/input/touch/TouchEvents.h>

namespace prev_test::scene::robot {
class CubeRobot final : public CubeRobotPart {
public:
    CubeRobot(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath);

    virtual ~CubeRobot() = default;

public:
    void Init() override;

    void Update(float deltaTime) override;

    void ShutDown() override;

public:
    void operator()(const prev::input::keyboard::KeyEvent& keyEvent);

    void operator()(const prev::input::mouse::MouseEvent& mouseEvent);

    void operator()(const prev::input::touch::TouchEvent& touchEvent);

private:
    prev::event::EventHandler<CubeRobot, prev::input::keyboard::KeyEvent> m_keyEvent{ *this };

    prev::event::EventHandler<CubeRobot, prev::input::mouse::MouseEvent> m_mouseEvent{ *this };

    prev::event::EventHandler<CubeRobot, prev::input::touch::TouchEvent> m_touchEvent{ *this };

private:
    std::shared_ptr<CubeRobotPart> m_body;

    std::shared_ptr<CubeRobotPart> m_head;

    std::shared_ptr<CubeRobotPart> m_leftArm;

    std::shared_ptr<CubeRobotPart> m_rightArm;

    std::shared_ptr<CubeRobotPart> m_leftLeg;

    std::shared_ptr<CubeRobotPart> m_rightLeg;

private:
    glm::vec2 m_angularVelocity{ 0.1f, 0.1f };

    glm::vec2 m_prevMousePosition{ 0.0f, 0.0f };
};

} // namespace prev_test::scene::robot

#endif // !__CUBE_ROBOT_H__

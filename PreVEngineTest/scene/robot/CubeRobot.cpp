#include "CubeRobot.h"

#include "../../common/AssetManager.h"
#include "../../component/transform/ITransformComponent.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::robot {
CubeRobot::CubeRobot(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const std::string& texturePath)
    : CubeRobotPart(position, orientation, scale, texturePath)
{
}

void CubeRobot::Init()
{
    m_body = std::make_shared<CubeRobotPart>(glm::vec3(0, 35, 0), glm::quat(1, 0, 0, 0), glm::vec3(10, 15, 5), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/vulkan.png"));

    m_head = std::make_shared<CubeRobotPart>(glm::vec3(0, 10, 0), glm::quat(1, 0, 0, 0), glm::vec3(5, 5, 5), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
    m_leftArm = std::make_shared<CubeRobotPart>(glm::vec3(-8, 10, -1), glm::quat(1, 0, 0, 0), glm::vec3(3, 18, 5), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
    m_rightArm = std::make_shared<CubeRobotPart>(glm::vec3(8, 10, -1), glm::quat(1, 0, 0, 0), glm::vec3(3, 18, 5), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
    m_leftLeg = std::make_shared<CubeRobotPart>(glm::vec3(-4, -12, 0), glm::quat(1, 0, 0, 0), glm::vec3(2.5, 17.5f, 4.7f), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));
    m_rightLeg = std::make_shared<CubeRobotPart>(glm::vec3(4, -12, 0), glm::quat(1, 0, 0, 0), glm::vec3(2.5, 17.5f, 4.7f), prev_test::common::AssetManager::Instance().GetAssetPath("Textures/texture.jpg"));

    m_body->AddChild(m_head);
    m_body->AddChild(m_leftArm);
    m_body->AddChild(m_rightArm);
    m_body->AddChild(m_leftLeg);
    m_body->AddChild(m_rightLeg);

    AddChild(m_body);

    CubeRobotPart::Init();
}

void CubeRobot::Update(float deltaTime)
{
    auto bodyTransformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(m_body->GetId());
    bodyTransformComponent->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(m_angularVelocity.x), glm::vec3(1.0f, 0.0f, 0.0f)));
    bodyTransformComponent->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(m_angularVelocity.y), glm::vec3(0.0f, 1.0f, 0.0f)));

    auto headTransformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(m_head->GetId());
    headTransformComponent->Rotate(glm::rotate(glm::mat4(1.0f), -glm::radians(25.0f) * deltaTime, glm::vec3(0, 1, 0)));

    auto leftArmTransformComponent = prev::scene::component::ComponentRepository<prev_test::component::transform::ITransformComponent>::Instance().Get(m_leftArm->GetId());
    leftArmTransformComponent->Translate(glm::vec3(0, -4.5, 0));
    leftArmTransformComponent->Rotate(glm::rotate(glm::mat4(1.0f), glm::radians(20.0f) * deltaTime, glm::vec3(1, 0, 0)));
    leftArmTransformComponent->Translate(glm::vec3(0, 4.5, 0));

    CubeRobotPart::Update(deltaTime);
}

void CubeRobot::ShutDown()
{
    CubeRobotPart::ShutDown();
}

void CubeRobot::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS) {
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_Left) {
            m_angularVelocity.y -= 0.1f;
        }

        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_Right) {
            m_angularVelocity.y += 0.1f;
        }

        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_Up) {
            m_angularVelocity.x += 0.1f;
        }

        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_Down) {
            m_angularVelocity.x -= 0.1f;
        }
    }
}

void CubeRobot::operator()(const prev::input::mouse::MouseEvent& mouseEvent)
{
    if (mouseEvent.action == prev::input::mouse::MouseActionType::MOVE && mouseEvent.button == prev::input::mouse::MouseButtonType::RIGHT) {
        m_angularVelocity = glm::vec2(mouseEvent.position.x - m_prevMousePosition.x, m_prevMousePosition.y - mouseEvent.position.y);
    }

    m_prevMousePosition = mouseEvent.position;
}

void CubeRobot::operator()(const prev::input::touch::TouchEvent& touchEvent)
{
    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE) {
        m_angularVelocity = glm::vec2(touchEvent.position.x - m_prevMousePosition.x, m_prevMousePosition.y - touchEvent.position.y);
    }

    m_prevMousePosition = touchEvent.position;
}
} // namespace prev_test::scene::robot
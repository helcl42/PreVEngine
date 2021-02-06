#include "Player.h"
#include "../common/AssetManager.h"
#include "../component/camera/CameraComponentFactory.h"
#include "../component/ray_casting/BoundingVolumeComponentFactory.h"
#include "../component/render/RenderComponentFactory.h"
#include "../component/terrain/ITerrainManagerComponent.h"
#include "../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/MathUtils.h>

namespace prev_test::scene {
Player::Player(const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
    : SceneNode()
    , m_initialPosition(position)
    , m_initialOrientation(orientation)
    , m_initialScale(scale)
{
}

void Player::Init()
{
    prev_test::component::transform::TrasnformComponentFactory transformComponentFactory{};
    m_transformComponent = transformComponentFactory.Create(m_initialPosition, m_initialOrientation, m_initialScale);
    if (prev::scene::component::NodeComponentHelper::HasComponent<prev_test::component::transform::ITransformComponent>(GetParent())) {
        m_transformComponent->SetParent(prev::scene::component::NodeComponentHelper::GetComponent<prev_test::component::transform::ITransformComponent>(GetParent()));
    }
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, TAG_TRANSFORM_COMPONENT);

    prev_test::component::render::RenderComponentFactory renderComponentFactory{};
    //m_animatonRenderComponent = renderComponentFactory.CreateAnimatedModelRenderComponent(prev_test::common::AssetManager::Instance().GetAssetPath("Models/Xbot/XBot.fbx"), { prev_test::common::AssetManager::Instance().GetAssetPath("Models/Xbot/Walking.fbx"), prev_test::common::AssetManager::Instance().GetAssetPath("Models/Xbot/Jump.fbx") }, { glm::vec4(0.49f, 0.3f, 0.28f, 1.0f), glm::vec4(0.52f, 0.42f, 0.4f, 1.0f) }, true, true);
    m_animatonRenderComponent = renderComponentFactory.CreateAnimatedModelRenderComponent(prev_test::common::AssetManager::Instance().GetAssetPath("Models/Archer/erika_archer_bow_arrow.fbx"), { prev_test::common::AssetManager::Instance().GetAssetPath("Models/Archer/Walking.fbx"), prev_test::common::AssetManager::Instance().GetAssetPath("Models/Archer/Jumping.fbx") }, true, true);

    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::render::IAnimationRenderComponent>(GetThis(), m_animatonRenderComponent, TAG_ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT);

    prev_test::component::camera::CameraComponentFactory cameraFactory{};
    m_cameraComponent = cameraFactory.Create(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 60.0f, 180.0f));
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::camera::ICameraComponent>(GetThis(), m_cameraComponent, TAG_CAMERA_COMPONENT);

    prev_test::component::ray_casting::BoundingVolumeComponentFactory bondingVolumeFactory{};
    m_boundingVolumeComponent = bondingVolumeFactory.CreateAABB(m_animatonRenderComponent->GetModel()->GetMesh()->GetVertices());
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, TAG_BOUNDING_VOLUME_COMPONENT);

    m_cameraComponent->AddOrientation(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), m_cameraComponent->GetUpDirection())));
    m_cameraComponent->AddOrientation(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), m_cameraComponent->GetRightDirection())));

    SceneNode::Init();
}

void Player::Update(float deltaTime)
{
    const auto terrain = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::terrain::ITerrainManagerComponent>({ TAG_TERRAIN_MANAGER_COMPONENT });

    // set default animation
    m_animatonRenderComponent->SetCurrentAnimationIndex(WALKING_ANIMATION_INDEX);

    auto walkingAnimation{ m_animatonRenderComponent->GetAnimation(WALKING_ANIMATION_INDEX) };
    if ((m_shouldGoForward || m_shouldGoBackward || m_shouldGoLeft || m_shouldGoRight) && !m_isInTheAir) {
        walkingAnimation->SetState(prev_test::render::AnimationState::RUNNING);
        walkingAnimation->Update(m_shouldGoBackward ? -deltaTime : deltaTime);

        glm::vec3 positionOffset{ 0.0f };
        if (m_shouldGoForward) {
            positionOffset += deltaTime * prev::util::MathUtil::GetForwardVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
        }
        if (m_shouldGoBackward) {
            positionOffset -= deltaTime * prev::util::MathUtil::GetForwardVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
        }
        if (m_shouldGoLeft) {
            positionOffset += deltaTime * prev::util::MathUtil::GetRightVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
        }
        if (m_shouldGoRight) {
            positionOffset -= deltaTime * prev::util::MathUtil::GetRightVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
        }
        m_transformComponent->Translate(positionOffset);
    } else {
        walkingAnimation->SetState(prev_test::render::AnimationState::STOPPED);
        walkingAnimation->Update(deltaTime);
    }

    const auto currentPosition{ m_transformComponent->GetPosition() };

    float height{ 0.0f };
    terrain->GetHeightAt(currentPosition, height);

    auto jumpAnimation{ m_animatonRenderComponent->GetAnimation(JUMP_ANIMATION_INDEX) };
    if (m_isInTheAir) {
        m_animatonRenderComponent->SetCurrentAnimationIndex(JUMP_ANIMATION_INDEX);
        jumpAnimation->SetState(prev_test::render::AnimationState::RUNNING);
        jumpAnimation->Update(deltaTime);
        m_upwardSpeed += GRAVITY_Y * deltaTime;
        m_transformComponent->Translate(glm::vec3(0.0f, m_upwardSpeed, 0.0f));
        if (currentPosition.y < height) {
            m_transformComponent->SetPosition(glm::vec3(currentPosition.x, height, currentPosition.z));
            m_upwardSpeed = 0.0f;
            m_isInTheAir = false;
        }
    } else {
        m_transformComponent->SetPosition(glm::vec3(currentPosition.x, height, currentPosition.z));
        jumpAnimation->SetState(prev_test::render::AnimationState::STOPPED);
        jumpAnimation->Update(deltaTime);
    }

    if (m_shouldRotate) {
        const auto yawAmount{ YAW_TURN_SPEED * m_rotationAroundY * deltaTime };
        const auto pitchAmount{ PITCH_TURN_SPEED * m_pitchDiff * deltaTime };

        m_transformComponent->Rotate(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(yawAmount), glm::vec3(0.0f, 1.0f, 0.0f))));

        m_cameraComponent->AddYaw(yawAmount);
        m_cameraComponent->AddPitch(pitchAmount);

        m_rotationAroundY = 0.0f;
        m_pitchDiff = 0.0f;
    }

    m_transformComponent->Update(deltaTime);

    const glm::vec3 cameraPosition{ m_transformComponent->GetPosition() + (-m_cameraComponent->GetForwardDirection() * m_distanceFromPerson) + glm::vec3(0.0f, 8.0f, 0.0f) };
    m_cameraComponent->SetPosition(cameraPosition);

    m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

    SceneNode::Update(deltaTime);
}

void Player::ShutDown()
{
    SceneNode::ShutDown();
}

void Player::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    if (keyEvent.action == prev::input::keyboard::KeyActionType::PRESS) {
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_W) {
            m_shouldGoForward = true;
        }
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_S) {
            m_shouldGoBackward = true;
        }
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_A) {
            m_shouldGoLeft = true;
        }
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_D) {
            m_shouldGoRight = true;
        }
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_Space) {
            if (!m_isInTheAir) {
                m_upwardSpeed = JUMP_POWER;
                m_isInTheAir = true;
            }
        }
    } else if (keyEvent.action == prev::input::keyboard::KeyActionType::RELEASE) {
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_W) {
            m_shouldGoForward = false;
        }
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_S) {
            m_shouldGoBackward = false;
        }
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_A) {
            m_shouldGoLeft = false;
        }
        if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_D) {
            m_shouldGoRight = false;
        }
    }
}

void Player::operator()(const prev::input::mouse::MouseEvent& mouseEvent)
{
    if (mouseEvent.button == prev::input::mouse::MouseButtonType::LEFT) {
        if (mouseEvent.action == prev::input::mouse::MouseActionType::PRESS) {
            m_shouldRotate = true;
        } else if (mouseEvent.action == prev::input::mouse::MouseActionType::RELEASE) {
            m_shouldRotate = false;
        } else if (m_shouldRotate && mouseEvent.action == prev::input::mouse::MouseActionType::MOVE) {
            m_rotationAroundY = mouseEvent.position.x;
            m_pitchDiff = mouseEvent.position.y;
        }
    }
}

void Player::operator()(const prev::input::touch::TouchEvent& touchEvent)
{
#if defined(__ANDROID__)
    if (touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        const float MAX_RATIO_FOR_JUMP_CONTROL = 0.25f;
        const auto MAX_X = touchEvent.extent.x * MAX_RATIO_FOR_JUMP_CONTROL;
        const auto MAX_Y = touchEvent.extent.y * MAX_RATIO_FOR_JUMP_CONTROL;
        if (touchEvent.position.x < MAX_X && touchEvent.position.y < MAX_Y) {
            if (!m_isInTheAir) {
                m_upwardSpeed = JUMP_POWER;
                m_isInTheAir = true;
            }
        }
    }

    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE || touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        const float MAX_RATIO_FOR_MOVE_CONTROL = 0.35f;
        const auto MIN_X = touchEvent.extent.x - touchEvent.extent.x * MAX_RATIO_FOR_MOVE_CONTROL;
        const auto MAX_Y = touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
        const auto MIN_Y = touchEvent.extent.y - touchEvent.extent.y * MAX_RATIO_FOR_MOVE_CONTROL;
        if (touchEvent.position.x > MIN_X && touchEvent.position.y < MAX_Y) {
            m_shouldGoForward = true;
        }

        if (touchEvent.position.x > MIN_X && touchEvent.position.y > MIN_Y) {
            m_shouldGoBackward = true;
        }
    } else {
        m_shouldGoForward = false;
        m_shouldGoBackward = false;
        return;
    }
#endif
    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE) {
        const glm::vec2 angleInDegrees = (touchEvent.position - m_prevTouchPosition) * 0.1f;

        m_transformComponent->Rotate(glm::quat_cast(glm::rotate(glm::mat4(1.0f), glm::radians(angleInDegrees.x), glm::vec3(0.0f, 1.0f, 0.0f))));

        m_cameraComponent->AddYaw(angleInDegrees.x);
        m_cameraComponent->AddPitch(angleInDegrees.y);
    }

    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE || touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        m_prevTouchPosition = touchEvent.position;
    }
}

void Player::operator()(const prev::input::mouse::MouseScrollEvent& scrollEvent)
{
    m_distanceFromPerson += scrollEvent.delta;
}
} // namespace prev_test::scene
#include "Player.h"

#include "../Tags.h"
#include "../common/AssetManager.h"
#include "../component/camera/CameraComponentFactory.h"
#include "../component/ray_casting/BoundingVolumeComponentFactory.h"
#include "../component/render/RenderComponentFactory.h"
#include "../component/terrain/ITerrainManagerComponent.h"
#include "../component/transform/TransformComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>
#include <prev/util/MathUtils.h>

namespace prev_test::scene {
namespace {
    auto AddRemoveFlag = [](const uint32_t flags, const uint32_t flag, const bool add) {
        return add ? (flags | flag) : (flags & ~flag);
    };
} // namespace

Player::Player(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale)
    : SceneNode({ TAG_MAIN_CAMERA, TAG_PLAYER })
    , m_device{ device }
    , m_allocator{ allocator }
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
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::transform::ITransformComponent>(GetThis(), m_transformComponent, { TAG_TRANSFORM_COMPONENT });

    prev_test::component::render::RenderComponentFactory renderComponentFactory{ m_device, m_allocator };
    // m_animationRenderComponent = renderComponentFactory.CreateAnimatedModelRenderComponent(prev_test::common::AssetManager::Instance().GetAssetPath("Models/Xbot/XBot.fbx"), { prev_test::common::AssetManager::Instance().GetAssetPath("Models/Xbot/Walking.fbx"), prev_test::common::AssetManager::Instance().GetAssetPath("Models/Xbot/Jump.fbx") }, { glm::vec4(0.49f, 0.3f, 0.28f, 1.0f), glm::vec4(0.52f, 0.42f, 0.4f, 1.0f) }, true, true);
    m_animationRenderComponent = renderComponentFactory.CreateAnimatedModelRenderComponent(prev_test::common::AssetManager::Instance().GetAssetPath("Models/Archer/erika_archer_bow_arrow.fbx"), { prev_test::common::AssetManager::Instance().GetAssetPath("Models/Archer/Walking.fbx"), prev_test::common::AssetManager::Instance().GetAssetPath("Models/Archer/Jumping.fbx") }, true, true);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::render::IAnimationRenderComponent>(GetThis(), m_animationRenderComponent, { TAG_ANIMATION_NORMAL_MAPPED_RENDER_COMPONENT });

    bool fixedCameraUp{ true };
    prev_test::component::camera::CameraComponentFactory cameraFactory{};
    m_cameraComponent = cameraFactory.Create(m_initialOrientation * glm::quat(glm::radians(glm::vec3(0.0f, 180.0f, 0.0f))), glm::vec3(0.0f, 0.0f, 0.0f), fixedCameraUp);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::camera::ICameraComponent>(GetThis(), m_cameraComponent, { TAG_CAMERA_COMPONENT });

    prev_test::component::ray_casting::BoundingVolumeComponentFactory bondingVolumeFactory{ m_allocator };
    m_boundingVolumeComponent = bondingVolumeFactory.CreateOBB(m_animationRenderComponent->GetModel()->GetMesh(), glm::vec3(0.4f, 1.0f, 0.4f));
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::ray_casting::IBoundingVolumeComponent>(GetThis(), m_boundingVolumeComponent, { TAG_BOUNDING_VOLUME_COMPONENT });

    m_cameraComponent->AddPitch(glm::radians(m_cameraPitch));

    SceneNode::Init();
}

void Player::Update(float deltaTime)
{
    const auto terrain{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::terrain::ITerrainManagerComponent>(GetRoot(), { TAG_TERRAIN_MANAGER_COMPONENT }) };

    // set default animation
    m_animationRenderComponent->SetCurrentAnimationIndex(WALKING_ANIMATION_INDEX);

    auto walkingAnimation{ m_animationRenderComponent->GetAnimation(WALKING_ANIMATION_INDEX) };

    if (m_shouldRotate) {
        const auto pitchAmount{ glm::radians(PITCH_TURN_SPEED * m_pitchYawRollDiff.x * deltaTime) };
        const auto yawAmount{ glm::radians(YAW_TURN_SPEED * m_pitchYawRollDiff.y * deltaTime) };

        m_transformComponent->Rotate(glm::quat_cast(glm::rotate(glm::mat4(1.0f), yawAmount, glm::vec3(0.0f, 1.0f, 0.0f))));

        m_cameraComponent->AddYaw(yawAmount);
        m_cameraComponent->AddPitch(pitchAmount);

        m_pitchYawRollDiff = {};
    }

    if (m_moveFlags && !m_isInTheAir) {
        walkingAnimation->SetState(prev_test::render::AnimationState::RUNNING);
        walkingAnimation->Update(m_moveFlags & MovementFlags::MOVE_BACKWARD ? -deltaTime : deltaTime);

        glm::vec3 positionOffset{ 0.0f };
        if (m_moveFlags & MovementFlags::MOVE_FORWARD) {
            positionOffset -= deltaTime * prev::util::math::GetForwardVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
        }
        if (m_moveFlags & MovementFlags::MOVE_BACKWARD) {
            positionOffset += deltaTime * prev::util::math::GetForwardVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
        }
        if (m_moveFlags & MovementFlags::MOVE_LEFT) {
            positionOffset += deltaTime * prev::util::math::GetRightVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
        }
        if (m_moveFlags & MovementFlags::MOVE_RIGHT) {
            positionOffset -= deltaTime * prev::util::math::GetRightVector(m_transformComponent->GetOrientation()) * RUN_SPEED;
        }
        m_transformComponent->Translate(positionOffset);
    } else {
        walkingAnimation->SetState(prev_test::render::AnimationState::STOPPED);
        walkingAnimation->Update(deltaTime);
    }

    const auto currentPosition{ m_transformComponent->GetPosition() };

    float height{ 0.0f };
    terrain->GetHeightAt(currentPosition, height);

    auto jumpAnimation{ m_animationRenderComponent->GetAnimation(JUMP_ANIMATION_INDEX) };
    if (m_isInTheAir) {
        m_animationRenderComponent->SetCurrentAnimationIndex(JUMP_ANIMATION_INDEX);
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

    m_transformComponent->Update(deltaTime);

    const glm::vec3 cameraPosition{ m_transformComponent->GetPosition() + (-m_cameraComponent->GetForwardDirection() * m_cameraDistanceFromPerson) + m_cameraComponent->GetDefaultUpDirection() * m_cameraPositionOffset };
    m_cameraComponent->SetPosition(cameraPosition);

    m_boundingVolumeComponent->Update(m_transformComponent->GetWorldTransformScaled());

    SceneNode::Update(deltaTime);
}

void Player::ShutDown()
{
    SceneNode::ShutDown();
}

void Player::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    const glm::ivec2 newResolution{ newIterationEvent.windowWidth, newIterationEvent.windowHeight };
    if (newResolution != m_currentResolution) {
        const auto& currentFrustum{ m_cameraComponent->GetViewFrustum() };
        m_cameraComponent->SetViewFrustum(prev_test::render::ViewFrustum{ currentFrustum.GetVerticalFov(), static_cast<float>(newResolution.x) / static_cast<float>(newResolution.y), currentFrustum.GetNearClippingPlane(), currentFrustum.GetFarClippingPlane() });
        m_currentResolution = newResolution;
    }
}

void Player::operator()(const prev::input::keyboard::KeyEvent& keyEvent)
{
    const bool press{ keyEvent.action == prev::input::keyboard::KeyActionType::PRESS };

    // move
    if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_W) {
        m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_FORWARD, press);
    }
    if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_S) {
        m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_BACKWARD, press);
    }
    if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_A) {
        m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_LEFT, press);
    }
    if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_D) {
        m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_RIGHT, press);
    }
    if (keyEvent.keyCode == prev::input::keyboard::KeyCode::KEY_Space) {
        if (!m_isInTheAir) {
            m_upwardSpeed = JUMP_POWER;
            m_isInTheAir = true;
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
        }
    }

    if (m_shouldRotate && mouseEvent.action == prev::input::mouse::MouseActionType::MOVE) {
        m_pitchYawRollDiff.y += mouseEvent.position.x;
        m_pitchYawRollDiff.x += -mouseEvent.position.y;
    }
}

void Player::operator()(const prev::input::touch::TouchEvent& touchEvent)
{
    if (touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        const float MAX_RATIO_FOR_JUMP_CONTROL{ 0.25f };
        const auto MaxPoint{ touchEvent.extent * MAX_RATIO_FOR_JUMP_CONTROL };
        if (touchEvent.position.x < MaxPoint.x && touchEvent.position.y < MaxPoint.y) {
            if (!m_isInTheAir) {
                m_upwardSpeed = JUMP_POWER;
                m_isInTheAir = true;
            }
        }
    }

    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE || touchEvent.action == prev::input::touch::TouchActionType::DOWN) {
        const float MAX_RATIO_FOR_MOVE_CONTROL{ 0.25f };
        const auto MinPoint{ touchEvent.extent - touchEvent.extent * MAX_RATIO_FOR_MOVE_CONTROL };
        const auto MaxPoint{ touchEvent.extent * MAX_RATIO_FOR_MOVE_CONTROL };
        if (touchEvent.position.x > MinPoint.x && touchEvent.position.y < MaxPoint.y) {
            m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_FORWARD, true);
            m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_BACKWARD, false);
        }

        if (touchEvent.position.x > MinPoint.x && touchEvent.position.y > MinPoint.y) {
            m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_FORWARD, false);
            m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_BACKWARD, true);
        }
    } else {
        m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_FORWARD, false);
        m_moveFlags = AddRemoveFlag(m_moveFlags, MovementFlags::MOVE_BACKWARD, false);
    }

    if (touchEvent.action == prev::input::touch::TouchActionType::MOVE) {
        const glm::vec2 angles{ (touchEvent.position - m_prevTouchPosition) * 1.0f };
        m_pitchYawRollDiff.y += angles.x;
        m_pitchYawRollDiff.x += -angles.y;
        m_shouldRotate = true;
    } else {
        m_shouldRotate = false;
    }

    m_prevTouchPosition = touchEvent.position;
}

void Player::operator()(const prev::input::mouse::MouseScrollEvent& scrollEvent)
{
    m_cameraDistanceFromPerson += static_cast<float>(scrollEvent.delta);
}
} // namespace prev_test::scene

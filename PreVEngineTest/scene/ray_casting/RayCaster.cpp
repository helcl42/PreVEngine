#include "RayCaster.h"
#include "../../component/camera/ICameraComponent.h"
#include "../../component/ray_casting/RayCasterComponentFactory.h"
#include "../../component/transform/ITransformComponent.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::ray_casting {
RayCaster::RayCaster()
    : SceneNode()
{
}

void RayCaster::Init()
{
    prev_test::component::ray_casting::RayCasterComponentFactory raycasterFactory{};
    m_rayCasterComponent = raycasterFactory.CreateRayCaster();
    m_mouseRayCasterComponent = raycasterFactory.CreateMouseRayCaster();

    AddRayCastComponent(m_inputFacade.IsMouseLocked());

    SceneNode::Init();
}

void RayCaster::Update(float deltaTime)
{
    const auto cameraComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA });
    const auto playerTransformComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::transform::ITransformComponent>({ TAG_PLAYER });

    if (m_inputFacade.IsMouseLocked()) {
        m_rayCasterComponent->SetRayLength(RAY_LENGTH);
        m_rayCasterComponent->SetRayStartPosition(playerTransformComponent->GetPosition() + glm::vec3(0.0f, 8.0f, 0.0f));
        m_rayCasterComponent->SetRayDirection(cameraComponent->GetForwardDirection());
        m_rayCasterComponent->SetOrientationOffsetAngles({ -12.0f, 0.0f });
        m_rayCasterComponent->Update(deltaTime);
    } else {
        m_mouseRayCasterComponent->SetRayLength(RAY_LENGTH);
        m_mouseRayCasterComponent->SetViewPortDimensions(m_viewPortSize);
        m_mouseRayCasterComponent->SetProjectionMatrix(cameraComponent->GetViewFrustum().CreateProjectionMatrix(m_viewPortSize.x / m_viewPortSize.y));
        m_mouseRayCasterComponent->SetViewMatrix(cameraComponent->LookAt());
        m_mouseRayCasterComponent->SetRayStartPosition(cameraComponent->GetPosition());
        m_mouseRayCasterComponent->Update(deltaTime);
    }
}

void RayCaster::ShutDown()
{
    SceneNode::ShutDown();
}

void RayCaster::RemoveRayCastComponnet()
{
    if (prev::scene::component::NodeComponentHelper::HasComponent<SceneNodeFlags, prev_test::component::ray_casting::IRayCasterComponent>(GetThis())) {
        prev::scene::component::NodeComponentHelper::RemoveComponent<SceneNodeFlags, prev_test::component::ray_casting::IRayCasterComponent>(GetThis(), SceneNodeFlags::RAYCASTER_COMPONENT);
    }
}

void RayCaster::AddRayCastComponent(const bool mouseLocked)
{
    if (mouseLocked) {
        prev::scene::component::NodeComponentHelper::AddComponent<SceneNodeFlags, prev_test::component::ray_casting::IRayCasterComponent>(GetThis(), m_rayCasterComponent, SceneNodeFlags::RAYCASTER_COMPONENT);
    } else {
        prev::scene::component::NodeComponentHelper::AddComponent<SceneNodeFlags, prev_test::component::ray_casting::IRayCasterComponent>(GetThis(), m_mouseRayCasterComponent, SceneNodeFlags::RAYCASTER_COMPONENT);
    }
}

void RayCaster::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
}

void RayCaster::operator()(const prev::input::mouse::MouseLockRequest& lockRequest)
{
    RemoveRayCastComponnet();
    AddRayCastComponent(lockRequest.lock);
}
} // namespace prev_test::scene::ray_casting
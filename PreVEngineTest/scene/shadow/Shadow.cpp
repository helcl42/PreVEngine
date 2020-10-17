#include "Shadow.h"
#include "../../component/camera/ICameraComponent.h"
#include "../../component/light/ILightComponent.h"
#include "../../component/shadow/ShadowsComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::shadow {
Shadows::Shadows()
    : SceneNode()
{
}

void Shadows::Init()
{
    prev_test::component::shadow::ShadowsComponentFactory shadowsFactory{};
    m_shadowsCompoent = shadowsFactory.Create();
    m_shadowsCompoent->Init();
    prev::scene::component::NodeComponentHelper::AddComponent<SceneNodeFlags, prev_test::component::shadow::IShadowsComponent>(GetThis(), m_shadowsCompoent, SceneNodeFlags::SHADOWS_COMPONENT);

    SceneNode::Init();
}

void Shadows::Update(float deltaTime)
{
    const auto lightComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto cameraComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA });

    m_shadowsCompoent->Update(lightComponent->GetDirection(), lightComponent->GetViewFrustum().GetNearClippingPlane(), lightComponent->GetViewFrustum().GetFarClippingPlane(), lightComponent->GetViewFrustum().CreateProjectionMatrix(1.0f), cameraComponent->LookAt());

    SceneNode::Update(deltaTime);
}

void Shadows::ShutDown()
{
    SceneNode::ShutDown();

    m_shadowsCompoent->ShutDown();
}
} // namespace prev_test::scene::shadow
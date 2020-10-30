#include "Sun.h"
#include "../../component/camera/ICameraComponent.h"
#include "../../component/light/ILightComponent.h"
#include "../../component/sky/SunComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::sky {
Sun::Sun()
    : SceneNode()
{
}

void Sun::Init()
{
    prev_test::component::sky::SunComponentFactory componentFactory{};
    m_sunComponent = std::move(componentFactory.Create());
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::sky::ISunComponent>(GetThis(), m_sunComponent, TAG_SUN_RENDER_COMPONENT);

    SceneNode::Init();
}

void Sun::Update(float deltaTime)
{
    const auto lightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto cameraComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA });

    m_sunComponent->Update(cameraComponent->GetViewFrustum().CreateProjectionMatrix(m_viewPortSize.x / m_viewPortSize.y), cameraComponent->LookAt(), cameraComponent->GetPosition(), lightComponent->GetPosition());

    SceneNode::Update(deltaTime);
}

void Sun::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::sky

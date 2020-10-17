#include "LensFlare.h"
#include "../../component/camera/ICameraComponent.h"
#include "../../component/light/ILightComponent.h"
#include "../../component/sky/LensFlareComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::sky {
LensFlare::LensFlare()
    : SceneNode()
{
}

void LensFlare::Init()
{
    prev_test::component::sky::LensFlareComponentFactory componentFactory{};
    m_lensFlareComponent = std::move(componentFactory.Create());
    prev::scene::component::NodeComponentHelper::AddComponent<SceneNodeFlags, prev_test::component::sky::ILensFlareComponent>(GetThis(), m_lensFlareComponent, SceneNodeFlags::LENS_FLARE_RENDER_COMPONENT);

    SceneNode::Init();
}

void LensFlare::Update(float deltaTime)
{
    const auto lightComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto cameraComponent = prev::scene::component::NodeComponentHelper::FindOne<SceneNodeFlags, prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA });

    m_lensFlareComponent->Update(cameraComponent->GetViewFrustum().CreateProjectionMatrix(m_viewPortSize.x / m_viewPortSize.y), cameraComponent->LookAt(), cameraComponent->GetPosition(), lightComponent->GetPosition());

    SceneNode::Update(deltaTime);
}

void LensFlare::ShutDown()
{
    SceneNode::ShutDown();
}

void LensFlare::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
}
} // namespace prev_test::scene::sky

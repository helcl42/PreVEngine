#include "Shadow.h"

#include "../../component/camera/ICameraComponent.h"
#include "../../component/light/ILightComponent.h"
#include "../../component/shadow/ShadowsComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::shadow {
Shadows::Shadows(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
{
}

void Shadows::Init()
{
    prev_test::component::shadow::ShadowsComponentFactory shadowsFactory{ m_device, m_allocator };
    auto shadowsCompoent{ shadowsFactory.Create() };
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::shadow::IShadowsComponent>(GetThis(), std::move(shadowsCompoent), TAG_SHADOWS_COMPONENT);

    SceneNode::Init();
}

void Shadows::Update(float deltaTime)
{
    const auto lightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto cameraComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA });

    auto shadowsComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>({ TAG_SHADOWS_COMPONENT }) };
    shadowsComponent->Update(lightComponent->GetDirection(), lightComponent->GetViewFrustum(), cameraComponent->LookAt());

    SceneNode::Update(deltaTime);
}

void Shadows::ShutDown()
{
    prev::scene::component::NodeComponentHelper::RemoveComponents<prev_test::component::shadow::IShadowsComponent>(GetThis(), TAG_SHADOWS_COMPONENT);

    SceneNode::ShutDown();
}
} // namespace prev_test::scene::shadow
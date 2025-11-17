#include "Shadow.h"

#include "../../Tags.h"
#include "../../component/camera/ICameraComponent.h"
#include "../../component/light/ILightComponent.h"
#include "../../component/shadow/ShadowsComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::shadow {
Shadows::Shadows(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : SceneNode({ TAG_SHADOW })
    , m_device{ device }
    , m_allocator{ allocator }
{
}

void Shadows::Init()
{
    prev_test::component::shadow::ShadowsComponentFactory shadowsFactory{ m_device, m_allocator };
    auto shadowsCompoent{ shadowsFactory.Create() };
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::shadow::IShadowsComponent>(GetThis(), std::move(shadowsCompoent), { TAG_SHADOWS_COMPONENT });

    SceneNode::Init();
}

void Shadows::Update(float deltaTime)
{
    const auto rootNode{ GetRoot() };

    const auto lightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>(rootNode, { TAG_MAIN_LIGHT });
    const auto cameraComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::camera::ICameraComponent>(rootNode, { TAG_MAIN_CAMERA });

    auto shadowsComponent{ prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::shadow::IShadowsComponent>(rootNode, { TAG_SHADOWS_COMPONENT }) };
    shadowsComponent->Update(lightComponent->GetDirection(), lightComponent->GetViewFrustum(), cameraComponent->LookAt());

    SceneNode::Update(deltaTime);
}

void Shadows::ShutDown()
{
    // TODO -> do I need to do this manually ? -> compoent repository should be destroyed with node
    prev::scene::component::NodeComponentHelper::RemoveComponents<prev_test::component::shadow::IShadowsComponent>(GetThis());

    SceneNode::ShutDown();
}
} // namespace prev_test::scene::shadow
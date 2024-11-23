#include "Sun.h"

#include "../../component/camera/ICameraComponent.h"
#include "../../component/light/ILightComponent.h"
#include "../../component/sky/SunComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::sky {
Sun::Sun(prev::core::device::Device& device, prev::core::memory::Allocator& allocator, uint32_t viewCount)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
    , m_viewCount{ viewCount }
{
}

void Sun::Init()
{
    for(uint32_t view = 0; view < m_viewCount; ++view) {
        std::shared_ptr<prev_test::component::sky::ISunComponent> sunComponent = prev_test::component::sky::SunComponentFactory{m_device, m_allocator}.Create();
        prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::sky::ISunComponent>(GetThis(), sunComponent, TAG_SUN_RENDER_COMPONENT);
        m_sunComponents.push_back(sunComponent);
    }

    SceneNode::Init();
}

void Sun::Update(float deltaTime)
{
    const auto lightComponent = prev::scene::component::NodeComponentHelper::FindOne<prev_test::component::light::ILightComponent>({ TAG_MAIN_LIGHT });
    const auto cameraComponents = prev::scene::component::NodeComponentHelper::FindAll<prev_test::component::camera::ICameraComponent>({ TAG_MAIN_CAMERA });

    for(uint32_t view = 0; view < m_viewCount; ++view) {
        const auto& cameraComponent{ cameraComponents[view] };
        const auto& sunComponent{ m_sunComponents[view] };
        sunComponent->Update(cameraComponent->GetViewFrustum().CreateProjectionMatrix(), cameraComponent->LookAt(), cameraComponent->GetPosition(), lightComponent->GetPosition());
    }

    SceneNode::Update(deltaTime);
}

void Sun::ShutDown()
{
    SceneNode::ShutDown();
}

void Sun::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
}
} // namespace prev_test::scene::sky

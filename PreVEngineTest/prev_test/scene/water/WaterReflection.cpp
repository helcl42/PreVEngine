#include "WaterReflection.h"

#include "../../component/common/OffScreenRenderPassComponentFactory.h"
#include "../../component/water/WaterCommon.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::water {
WaterReflection::WaterReflection()
    : SceneNode()
{
}

void WaterReflection::Init()
{
    m_viewPortSize = m_previousViewPortSize;
    CreateReflectionComponent();

    SceneNode::Init();
}

void WaterReflection::Update(float deltaTime)
{
    if (m_viewPortSize != m_previousViewPortSize) {
        DestroyReflectionComponent();
        CreateReflectionComponent();
        m_previousViewPortSize = m_viewPortSize;
    }

    SceneNode::Update(deltaTime);
}

void WaterReflection::ShutDown()
{
    SceneNode::ShutDown();

    m_reflectionComponent->ShutDown();
}

void WaterReflection::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
}

void WaterReflection::CreateReflectionComponent()
{
    const VkExtent2D extent{ m_viewPortSize.x / prev_test::component::water::REFLECTION_EXTENT_DIVIDER, m_viewPortSize.y / prev_test::component::water::REFLECTION_EXTENT_DIVIDER };

    prev_test::component::common::OffScreenRenderPassComponentFactory componentFactory{};
    m_reflectionComponent = componentFactory.Create(extent, VK_FORMAT_D32_SFLOAT, { VK_FORMAT_B8G8R8A8_UNORM });
    m_reflectionComponent->Init();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::common::IOffScreenRenderPassComponent>(GetThis(), m_reflectionComponent, TAG_WATER_REFLECTION_RENDER_COMPONENT);
}

void WaterReflection::DestroyReflectionComponent()
{
    if (m_reflectionComponent) {
        prev::scene::component::NodeComponentHelper::RemoveComponents<prev_test::component::common::IOffScreenRenderPassComponent>(GetThis(), TAG_WATER_REFLECTION_RENDER_COMPONENT);
        m_reflectionComponent->ShutDown();
    }
}

} // namespace prev_test::scene::water

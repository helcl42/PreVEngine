#include "WaterReflection.h"

#include "../../component/water/WaterCommon.h"
#include "../../component/water/WaterComponentFactory.h"

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
    prev_test::component::water::WaterComponentFactory componentFactory{};
    m_reflectionComponent = componentFactory.CreateOffScreenComponent(m_viewPortSize.x / prev_test::component::water::REFLECTION_EXTENT_DIVIDER, m_viewPortSize.y / prev_test::component::water::REFLECTION_EXTENT_DIVIDER);
    m_reflectionComponent->Init();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::water::IWaterOffscreenRenderPassComponent>(GetThis(), m_reflectionComponent, TAG_WATER_REFLECTION_RENDER_COMPONENT);
}

void WaterReflection::DestroyReflectionComponent()
{
    if (m_reflectionComponent) {
        prev::scene::component::NodeComponentHelper::RemoveComponent<prev_test::component::water::IWaterOffscreenRenderPassComponent>(GetThis(), TAG_WATER_REFLECTION_RENDER_COMPONENT);
        m_reflectionComponent->ShutDown();
    }
}

} // namespace prev_test::scene::water

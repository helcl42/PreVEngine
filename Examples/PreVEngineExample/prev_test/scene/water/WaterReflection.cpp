#include "WaterReflection.h"

#include "../../Tags.h"
#include "../../component/common/OffScreenRenderPassComponentFactory.h"
#include "../../component/water/WaterCommon.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::water {
WaterReflection::WaterReflection(prev::core::device::Device& device, uint32_t viewCount)
    : SceneNode()
    , m_device{ device }
    , m_viewCount{ viewCount }
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

    m_reflectionComponent.reset();
}

void WaterReflection::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
}

void WaterReflection::CreateReflectionComponent()
{
    const GfxExtent2D extent{ m_viewPortSize.x / prev_test::component::water::REFLECTION_EXTENT_DIVIDER, m_viewPortSize.y / prev_test::component::water::REFLECTION_EXTENT_DIVIDER };

    prev_test::component::common::OffScreenRenderPassComponentFactory componentFactory{ m_device };
    m_reflectionComponent = componentFactory.Create(extent, GFX_FORMAT_DEPTH32_FLOAT, { GFX_FORMAT_B8G8R8A8_UNORM }, m_viewCount);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::common::IOffScreenRenderPassComponent>(GetThis(), m_reflectionComponent, { TAG_WATER_REFLECTION_RENDER_COMPONENT });
}

void WaterReflection::DestroyReflectionComponent()
{
    if (m_reflectionComponent) {
        prev::scene::component::NodeComponentHelper::RemoveComponents<prev_test::component::common::IOffScreenRenderPassComponent>(GetThis(), { TAG_WATER_REFLECTION_RENDER_COMPONENT });
    }
}

} // namespace prev_test::scene::water

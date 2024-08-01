#include "WaterRefraction.h"

#include "../../component/common/OffScreenRenderPassComponentFactory.h"
#include "../../component/water/WaterCommon.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::water {
WaterRefraction::WaterRefraction(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
{
}

void WaterRefraction::Init()
{
    m_viewPortSize = m_previousViewPortSize;
    CreateRefractionComponent();

    SceneNode::Init();
}

void WaterRefraction::Update(float deltaTime)
{
    if (m_viewPortSize != m_previousViewPortSize) {
        DestroyRefractionComponent();
        CreateRefractionComponent();
        m_previousViewPortSize = m_viewPortSize;
    }

    SceneNode::Update(deltaTime);
}

void WaterRefraction::ShutDown()
{
    SceneNode::ShutDown();

    m_refractionComponent->ShutDown();
}

void WaterRefraction::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
}

void WaterRefraction::CreateRefractionComponent()
{
    const VkExtent2D extent{ m_viewPortSize.x / prev_test::component::water::REFRACTION_EXTENT_DIVIDER, m_viewPortSize.y / prev_test::component::water::REFRACTION_EXTENT_DIVIDER };

    prev_test::component::common::OffScreenRenderPassComponentFactory componentFactory{ m_device, m_allocator };
    m_refractionComponent = componentFactory.Create(extent, VK_FORMAT_D32_SFLOAT, { VK_FORMAT_B8G8R8A8_UNORM });
    m_refractionComponent->Init();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::common::IOffScreenRenderPassComponent>(GetThis(), m_refractionComponent, TAG_WATER_REFRACTION_RENDER_COMPONENT);
}

void WaterRefraction::DestroyRefractionComponent()
{
    if (m_refractionComponent) {
        prev::scene::component::NodeComponentHelper::RemoveComponents<prev_test::component::common::IOffScreenRenderPassComponent>(GetThis(), TAG_WATER_REFRACTION_RENDER_COMPONENT);
        m_refractionComponent->ShutDown();
    }
}
} // namespace prev_test::scene::water

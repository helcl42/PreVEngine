#include "WaterRefraction.h"

#include "../../component/water/WaterCommon.h"
#include "../../component/water/WaterComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::water {
WaterRefraction::WaterRefraction()
    : SceneNode()
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
    prev_test::component::water::WaterComponentFactory componentFactory{};
    m_refractionComponent = componentFactory.CreateOffScreenComponent(m_viewPortSize.x / prev_test::component::water::REFRACTION_EXTENT_DIVIDER, m_viewPortSize.y / prev_test::component::water::REFRACTION_EXTENT_DIVIDER);
    m_refractionComponent->Init();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::water::IWaterOffscreenRenderPassComponent>(GetThis(), m_refractionComponent, TAG_WATER_REFRACTION_RENDER_COMPONENT);
}

void WaterRefraction::DestroyRefractionComponent()
{
    if (m_refractionComponent) {
        prev::scene::component::NodeComponentHelper::RemoveComponent<prev_test::component::water::IWaterOffscreenRenderPassComponent>(GetThis(), TAG_WATER_REFRACTION_RENDER_COMPONENT);
        m_refractionComponent->ShutDown();
    }
}
} // namespace prev_test::scene::water

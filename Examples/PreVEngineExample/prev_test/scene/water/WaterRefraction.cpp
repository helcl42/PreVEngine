#include "WaterRefraction.h"

#include "../../Tags.h"
#include "../../component/common/OffScreenRenderPassComponentFactory.h"
#include "../../component/water/WaterCommon.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::water {
WaterRefraction::WaterRefraction(prev::core::device::Device& device)
    : SceneNode()
    , m_device{ device }
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
    const GfxExtent2D extent{ m_viewPortSize.x / prev_test::component::water::REFRACTION_EXTENT_DIVIDER, m_viewPortSize.y / prev_test::component::water::REFRACTION_EXTENT_DIVIDER };
    if (m_viewPortSize != m_previousViewPortSize && extent.width > 0 && extent.height > 0) {
        DestroyRefractionComponent();
        CreateRefractionComponent();
        m_previousViewPortSize = m_viewPortSize;
    }

    SceneNode::Update(deltaTime);
}

void WaterRefraction::ShutDown()
{
    SceneNode::ShutDown();

    m_refractionComponent.reset();
}

void WaterRefraction::operator()(const prev::core::NewIterationEvent& newIterationEvent)
{
    m_viewPortSize = glm::vec2(newIterationEvent.windowWidth, newIterationEvent.windowHeight);
}

void WaterRefraction::CreateRefractionComponent()
{
    const GfxExtent2D extent{ m_viewPortSize.x / prev_test::component::water::REFRACTION_EXTENT_DIVIDER, m_viewPortSize.y / prev_test::component::water::REFRACTION_EXTENT_DIVIDER };

    prev_test::component::common::OffScreenRenderPassComponentFactory componentFactory{ m_device };
    // Layer count follows the shader view count (MAX_PER_PASS_VIEW_COUNT), not the XR eye count - see WaterReflection.
    m_refractionComponent = componentFactory.Create(extent, GFX_FORMAT_DEPTH32_FLOAT, { GFX_FORMAT_B8G8R8A8_UNORM }, MAX_PER_PASS_VIEW_COUNT_VALUE);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::common::IOffScreenRenderPassComponent>(GetThis(), m_refractionComponent, { TAG_WATER_REFRACTION_RENDER_COMPONENT });
}

void WaterRefraction::DestroyRefractionComponent()
{
    if (m_refractionComponent) {
        prev::scene::component::NodeComponentHelper::RemoveComponents<prev_test::component::common::IOffScreenRenderPassComponent>(GetThis(), { TAG_WATER_REFRACTION_RENDER_COMPONENT });
        m_refractionComponent.reset(); // keep the guard honest: without this the stale non-null pointer lets a
                                       // second Destroy (e.g. re-entrant Update while Create's async GPU work is
                                       // suspended on WebGPU) call Remove on an already-empty repository, which throws.
    }
}
} // namespace prev_test::scene::water

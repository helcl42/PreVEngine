#include "LensFlare.h"

#include "../../Tags.h"
#include "../../component/camera/ICameraComponent.h"
#include "../../component/light/ILightComponent.h"
#include "../../component/sky/LensFlareComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::sky {
LensFlare::LensFlare(prev::core::device::Device& device)
    : SceneNode()
    , m_device{ device }
{
}

void LensFlare::Init()
{
    m_lensFlareComponent = prev_test::component::sky::LensFlareComponentFactory{ m_device }.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::sky::ILensFlareComponent>(GetThis(), m_lensFlareComponent, { TAG_LENS_FLARE_RENDER_COMPONENT });

    SceneNode::Init();
}

void LensFlare::Update(float deltaTime)
{
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

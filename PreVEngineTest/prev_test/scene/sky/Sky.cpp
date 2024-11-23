#include "Sky.h"

#include "../../component/sky/SkyComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::sky {
Sky::Sky(prev::core::device::Device& device, prev::core::memory::Allocator& allocator)
    : SceneNode()
    , m_device{ device }
    , m_allocator{ allocator }
{
}

void Sky::Init()
{
    SceneNode::Init();

    m_skyComponent = prev_test::component::sky::SkyComponentFactory{ m_device, m_allocator }.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::sky::ISkyComponent>(GetThis(), m_skyComponent, TAG_SKY_RENDER_COMPONENT);
}

void Sky::Update(float deltaTime)
{
    m_skyComponent->Update(deltaTime);

    SceneNode::Update(deltaTime);
}

void Sky::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::sky

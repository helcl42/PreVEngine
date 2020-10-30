#include "Sky.h"

#include "../../component/sky/SkyComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::sky {
Sky::Sky()
    : SceneNode()
{
}

void Sky::Init()
{
    SceneNode::Init();

    prev_test::component::sky::SkyComponentFactory skyComponentFactory{};
    std::shared_ptr<prev_test::component::sky::ISkyComponent> skyComponent = skyComponentFactory.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::sky::ISkyComponent>(GetThis(), skyComponent, TAG_SKY_RENDER_COMPONENT);
}

void Sky::Update(float deltaTime)
{
    SceneNode::Update(deltaTime);
}

void Sky::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::sky

#include "Clouds.h"

#include "../../component/cloud/CloudsComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene::sky {
Clouds::Clouds()
    : SceneNode()
{
}

void Clouds::Init()
{
    SceneNode::Init();

    prev_test::component::cloud::CloudsComponentFactory cloudsComponentFactory{};
    std::shared_ptr<prev_test::component::cloud::ICloudsComponent> cloudsComponent = cloudsComponentFactory.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<SceneNodeFlags, prev_test::component::cloud::ICloudsComponent>(GetThis(), cloudsComponent, SceneNodeFlags::CLOUDS_COMPONENT);
}

void Clouds::Update(float deltaTime)
{
    SceneNode::Update(deltaTime);
}

void Clouds::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene::sky

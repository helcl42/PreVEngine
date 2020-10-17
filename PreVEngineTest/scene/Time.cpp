#include "Time.h"

#include "../component/time/TimeComponentFactory.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene {
Time::Time()
    : SceneNode()
{
}

void Time::Init()
{
    prev_test::component::time::TimeComponentFactory timeComponentFactory{};
    m_timeComponent = timeComponentFactory.Create();
    prev::scene::component::NodeComponentHelper::AddComponent<SceneNodeFlags, prev_test::component::time::ITimeComponent>(GetThis(), m_timeComponent, SceneNodeFlags::TIME_COMPONENT);

    SceneNode::Init();
}

void Time::Update(float deltaTime)
{
    m_timeComponent->Update(deltaTime);

    SceneNode::Update(deltaTime);
}

void Time::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene
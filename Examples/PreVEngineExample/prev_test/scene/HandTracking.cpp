#include "HandTracking.h"

#ifdef ENABLE_XR

#include "../Tags.h"
#include "../component/hand_tracking/HandTrackingComponent.h"

#include <prev/scene/component/NodeComponentHelper.h>

namespace prev_test::scene {
HandTracking::HandTracking(prev::core::device::Device& device)
    : SceneNode()
    , m_device{ device }
{
}

void HandTracking::Init()
{
    auto handTrackingComponent = std::make_shared<prev_test::component::hand_tracking::HandTrackingComponent>(m_device);
    prev::scene::component::NodeComponentHelper::AddComponent<prev_test::component::hand_tracking::IHandTrackingComponent>(GetThis(), handTrackingComponent, { TAG_HAND_TRACKING_RENDER_COMPONENT });

    SceneNode::Init();
}

void HandTracking::Update(float deltaTime)
{
    SceneNode::Update(deltaTime);
}

void HandTracking::ShutDown()
{
    SceneNode::ShutDown();
}
} // namespace prev_test::scene

#endif

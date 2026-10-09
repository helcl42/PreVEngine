#include "XrEyes.h"

#ifdef ENABLE_XR

#include <prev/common/Common.h>
#include <prev/event/EventChannel.h>

#include <algorithm>
#include <utility>

namespace prev_test::scene {
XrEyes::XrEyes(std::vector<std::shared_ptr<prev_test::component::camera::ICameraComponent>> cameraComponents)
    : m_cameraComponents{ std::move(cameraComponents) }
{
}

void XrEyes::Update(float deltaTime)
{
    if (m_eyes) {
        const uint32_t viewCount{ std::min(m_eyes->count, static_cast<uint32_t>(m_cameraComponents.size())) };
        for (uint32_t view = 0; view < viewCount; ++view) {
            auto& cameraComponent{ m_cameraComponents[view] };
            const auto previousFrustum{ cameraComponent->GetViewFrustum() };

            const auto& fov{ m_eyes->fovs[view] };
            cameraComponent->SetViewFrustum(prev_test::render::ViewFrustum{ fov.angleLeft, fov.angleRight, fov.angleUp, fov.angleDown, previousFrustum.GetNearClippingPlane(), previousFrustum.GetFarClippingPlane() });

            const auto& pose{ m_eyes->poses[view] };
            cameraComponent->SetOrientation(pose.orientation);
            cameraComponent->SetPosition(pose.position);
        }

        const auto viewFrustum{ m_cameraComponents[0]->GetViewFrustum() };
        prev::event::EventChannel::Post(prev::xr::CameraFeedbackEvent{ viewFrustum.GetNearClippingPlane(), viewFrustum.GetFarClippingPlane(), MIN_DEPTH, MAX_DEPTH });
    }
    SceneNode::Update(deltaTime);
}

void XrEyes::operator()(const prev::xr::CameraEvent& cameraEvent)
{
    m_eyes = cameraEvent;
}

void XrEyes::operator()(const prev::xr::XrSessionChangedEvent& sessionEvent)
{
    m_eyes.reset(); // a new session's eyes arrive with its first head pose

    for (auto& cameraComponent : m_cameraComponents) {
        cameraComponent->SetUseFixedUp(!sessionEvent.running);
    }
}
} // namespace prev_test::scene

#endif // ENABLE_XR

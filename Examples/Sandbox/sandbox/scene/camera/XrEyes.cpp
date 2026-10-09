#include "XrEyes.h"

#ifdef ENABLE_XR

#include "Camera.h"

#include <prev/common/Common.h>
#include <prev/event/EventChannel.h>
#include <prev/util/MathUtils.h>

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <utility>

namespace sandbox::scene::camera {
XrEyes::XrEyes(std::shared_ptr<sandbox::component::CameraComponent> camera)
    : m_camera{ std::move(camera) }
    , m_maxViews{ m_camera->GetViewCount() }
{
}

void XrEyes::Update(float deltaTime)
{
    if (m_eyes) {
        const uint32_t viewCount{ std::min(m_eyes->count, m_maxViews) };
        m_camera->SetViewCount(viewCount);
        for (uint32_t view = 0; view < viewCount; ++view) {
            const auto& pose{ m_eyes->poses[view] };

            // The view (world->eye) matrix is the inverse of the eye's world transform.
            const glm::mat4 worldTransform{ glm::inverse(glm::translate(glm::mat4{ 1.0f }, pose.position) * glm::mat4_cast(pose.orientation)) };

            m_camera->SetPositions(view, pose.position);
            m_camera->SetViewMatrix(view, worldTransform);
            m_camera->SetProjectionMatrix(view, prev::util::math::CreatePerspectiveProjectionMatrix(m_eyes->fovs[view], Camera::NEAR_CLIPPING_PLANE, Camera::FAR_CLIPPING_PLANE));
        }

        // Tell the XR layer which clipping planes / depth range we render with, so its depth submission and reprojection match.
        prev::event::EventChannel::Post(prev::xr::CameraFeedbackEvent{ Camera::NEAR_CLIPPING_PLANE, Camera::FAR_CLIPPING_PLANE, MIN_DEPTH, MAX_DEPTH });
    }
    SceneNode::Update(deltaTime);
}

void XrEyes::operator()(const prev::xr::CameraEvent& cameraEvent)
{
    m_eyes = cameraEvent;
}

void XrEyes::operator()(const prev::xr::XrSessionChangedEvent&)
{
    m_eyes.reset(); // a new session's eyes arrive with its first head pose
}
} // namespace sandbox::scene::camera

#endif // ENABLE_XR

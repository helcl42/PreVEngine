#include "XrCamera.h"

#ifdef ENABLE_XR

#include <prev/common/Common.h>
#include <prev/event/EventChannel.h>
#include <prev/util/MathUtils.h>

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace sandbox::scene::camera {
XrCamera::XrCamera(uint32_t viewCount)
    : Camera{ viewCount }
{
}

void XrCamera::operator()(const prev::xr::CameraEvent& cameraEvent)
{
    const uint32_t viewCount{ std::min(cameraEvent.count, m_viewCount) };
    m_camera->SetViewCount(viewCount);
    for (uint32_t view = 0; view < viewCount; ++view) {
        const auto& pose{ cameraEvent.poses[view] };

        // The view (world->eye) matrix is the inverse of the eye's world transform.
        const glm::mat4 worldTransform{ glm::inverse(glm::translate(glm::mat4{ 1.0f }, pose.position) * glm::mat4_cast(pose.orientation)) };

        m_camera->SetPositions(view, pose.position);
        m_camera->SetViewMatrix(view, worldTransform);
        m_camera->SetProjectionMatrix(view, prev::util::math::CreatePerspectiveProjectionMatrix(cameraEvent.fovs[view], NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE));
    }

    // Tell the XR layer which clipping planes / depth range we render with, so its depth submission and reprojection match.
    prev::event::EventChannel::Post(prev::xr::CameraFeedbackEvent{ NEAR_CLIPPING_PLANE, FAR_CLIPPING_PLANE, MIN_DEPTH, MAX_DEPTH });
}
} // namespace sandbox::scene::camera

#endif // ENABLE_XR

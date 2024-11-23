#ifndef __XR_EVENTS_H__
#define __XR_EVENTS_H__

#include "../common/Common.h"
#include "../util/MathUtils.h"

namespace prev::xr {
    struct XrCameraEvent {
        prev::util::math::Pose poses[MAX_VIEW_COUNT] = {};
        prev::util::math::Fov fovs[MAX_VIEW_COUNT] = {};
        uint32_t count{};
    };

    struct XrCameraFeedbackEvent {
        float nearClippingPlane{};
        float fatClippingPlane{};
        float minDepth{};
        float maxDepth{};
    };
}

#endif
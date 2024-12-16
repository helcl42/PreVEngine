#ifndef __XR_EVENTS_H__
#define __XR_EVENTS_H__

#ifdef ENABLE_XR

#include "../common/Common.h"
#include "../common/FlagSet.h"
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

    enum class HandEventFlags : uint32_t {
        NONE = 0,
        SQUEEZE = 1,
        TRIGGER = 2,
        // add new flags
        _
    };

    enum class HandType : uint32_t {
        LEFT = 0,
        RIGHT = 1
    };

    struct XrHandEvent {
        HandType type{};
        bool active{};
        prev::util::math::Pose pose{};
        float squeeze{};
        prev::common::FlagSet<HandEventFlags> flags{};
    };

    struct XrHandsEvent {
        XrHandEvent hands[2] = {};
    };
}

#endif

#endif
#ifndef __XR_EVENTS_H__
#define __XR_EVENTS_H__

#ifdef ENABLE_XR

#include "../common/Common.h"
#include "../common/FlagSet.h"
#include "../util/MathUtils.h"

namespace prev::xr {

constexpr const uint32_t MAX_HAND_COUNT{ 2 };
constexpr const uint32_t MAX_HAND_TRACKING_JOINT_COUNT{ 26 };

struct CameraEvent {
    prev::util::math::Pose poses[MAX_VIEW_COUNT]{};
    prev::util::math::Fov fovs[MAX_VIEW_COUNT]{};
    uint32_t count{};
};

struct CameraFeedbackEvent {
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
    RIGHT = 1,
};

enum class HandJointType : uint32_t {
    PALM_EXT = 0,
    WRIST_EXT = 1,
    THUMB_METACARPAL_EXT = 2,
    THUMB_PROXIMAL_EXT = 3,
    THUMB_DISTAL_EXT = 4,
    THUMB_TIP_EXT = 5,
    INDEX_METACARPAL_EXT = 6,
    INDEX_PROXIMAL_EXT = 7,
    INDEX_INTERMEDIATE_EXT = 8,
    INDEX_DISTAL_EXT = 9,
    INDEX_TIP_EXT = 10,
    MIDDLE_METACARPAL_EXT = 11,
    MIDDLE_PROXIMAL_EXT = 12,
    MIDDLE_INTERMEDIATE_EXT = 13,
    MIDDLE_DISTAL_EXT = 14,
    MIDDLE_TIP_EXT = 15,
    RING_METACARPAL_EXT = 16,
    RING_PROXIMAL_EXT = 17,
    RING_INTERMEDIATE_EXT = 18,
    RING_DISTAL_EXT = 19,
    RING_TIP_EXT = 20,
    LITTLE_METACARPAL_EXT = 21,
    LITTLE_PROXIMAL_EXT = 22,
    LITTLE_INTERMEDIATE_EXT = 23,
    LITTLE_DISTAL_EXT = 24,
    LITTLE_TIP_EXT = 25,
};

struct HandJoint {
    HandJointType type{};
    bool active{};
    prev::util::math::Pose pose{};
    float radius{};
};

struct HandEvent {
    HandType type{};
    bool active{};
    HandJoint joints[MAX_HAND_TRACKING_JOINT_COUNT]{};
    float squeeze{};
    prev::util::math::Pose pose{};
};

struct HandsEvent {
    HandEvent hands[MAX_HAND_COUNT]{};
};

struct HandControllerEvent {
    HandType type{};
    bool active{};
    prev::util::math::Pose pose{};
    float squeeze{};
    prev::common::FlagSet<HandEventFlags> flags{};
};

struct HandControllersEvent {
    HandControllerEvent handControllers[MAX_HAND_COUNT]{};
};

struct HapticFeedback {
    HandType type{};
    float amplitude{};
    int64_t duration{};
};

} // namespace prev::xr

#endif

#endif
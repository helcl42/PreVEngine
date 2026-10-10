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
    prev::util::math::Pose poses[MAX_VIEW_COUNT_VALUE]{};
    prev::util::math::Fov fovs[MAX_VIEW_COUNT_VALUE]{};
    uint32_t count{};
};

struct CameraFeedbackEvent {
    float nearClippingPlane{};
    float fatClippingPlane{};
    float minDepth{};
    float maxDepth{};
};

// Client -> backend: request switching VR <-> AR (passthrough) at runtime (OpenXR supports in-session).
struct XrPassthroughChangeRequestEvent {
    bool enabled{};
};

// Backend -> clients: the actual mode after a request resolved (clamped to what the runtime supports).
struct XrPassthroughChangedEvent {
    bool enabled{};
};

// Engine -> clients: an immersive session started or ended (WebXR: Enter VR/AR, or leaving it).
struct XrSessionChangedEvent {
    bool running{};
};

// Engine -> clients: the running session gained or lost input focus (e.g. the system menu opened over it).
struct XrSessionFocusChangedEvent {
    bool focused{};
};

enum class HandEventFlags : uint32_t {
    NONE = 0,
    SQUEEZE = 1,
    TRIGGER = 2,
    PRIMARY = 3, // the lower face button (A, or X on a left Touch controller)
    SECONDARY = 4, // the upper face button (B, or Y on a left Touch controller)
    THUMBSTICK_CLICK = 5, // the stick pressed in
    MENU = 6,
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
    prev::util::math::Pose pose{};
};

struct HandsEvent {
    HandEvent hands[MAX_HAND_COUNT]{};
};

struct HandControllerEvent {
    HandType type{};
    bool active{};
    prev::util::math::Pose pose{};
    prev::util::math::Pose aimPose{};
    float squeeze{};
    float trigger{};
    glm::vec2 thumbstick{}; // [-1, 1]: x to the right, y forward
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